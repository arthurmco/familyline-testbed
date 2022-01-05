# Automatically guesses asset dependencies just from reading it.
#
# Some model files have indications of their texture and material files 
# embedded in them.
# This script extracts that information from all of Familyline's supported
# model files.
#
# Returns a part of a YAML that you can append into the original file.

import argparse
import logging
import sys
import os
from dataclasses import dataclass
from typing import Optional, List, Tuple
import yaml

@dataclass
class ModelTexture:
    name: str
    path: str

@dataclass
class ModelMaterial:
    name: Optional[str]
    path: Optional[str]
    textures: List[ModelTexture]

@dataclass
class ModelInfo:
    model_name: str
    model_path: str
    material: List[ModelMaterial]

    # The size is just for show, it is not really added to the file
    size: Optional[Tuple[float, float, float]]


def get_asset_name_from_path(path: str) -> str:
    _, tmp = os.path.split(path)
    name, _ = os.path.splitext(tmp)

    return name

def guess_without_texture_info(model: str, texture: str, masset: str, msize = None) -> ModelInfo:
    if texture is None:
        raise RuntimeError("You should specify a texture on formats without file info")

    tasset = get_asset_name_from_path(texture)

    logging.warning(f"{model} format does not have texture file information embedded")
    logging.warning("using the specified texture")

    return ModelInfo(masset, model, [
        ModelMaterial(None, None, [
            ModelTexture(tasset, texture)])], msize)


def calculate_size(points: List[Tuple[float, float, float]]) -> Tuple[float, float, float]:
    """
    Calculate the width and length of the mesh, based on the list of every
    point of the mesh
    """
    minx = 99999
    miny = 99999
    minz = 99999
    maxx = -minx
    maxy = -miny
    maxz = -minz

    for p in points:
        px, py, pz = p
        minx = min(minx, px)
        miny = min(miny, py)
        minz = min(minz, pz)
        maxx = max(maxx, px)
        maxy = max(maxy, py)
        maxz = max(maxz, pz)

    return (maxx-minx, maxy-miny, maxz-minz)

def guess_obj_file(model: str, texture: str, masset: str) -> ModelInfo:
    with open(model, "r") as modelfile:
        mtllib = None
        points = []
        
        for l in modelfile.readlines():
            if l.startswith("mtllib"):
                mtllib = l.replace("mtllib", "", 1).strip()
            elif l.startswith("v "):
                parts = l.split(" ")
                points.append((float(parts[1]), float(parts[2]), float(parts[3])))

        msize = calculate_size(points)
            
        # TODO: extract texture from material
        if mtllib is None:
            logging.warn(f"OBJ: {model} has no material embedded!")
            return guess_without_texture_info(model, texture, masset, msize)

        mtexes = []
        if texture is not None:
            tasset = get_asset_name_from_path(texture).lower()
            mtexes.append(ModelTexture(tasset, texture))

        mtlassetname, _ = os.path.splitext(mtllib)
        modelpath = os.path.dirname(model)

        return ModelInfo(masset, model, [
            ModelMaterial(mtlassetname, 
                          os.path.join(modelpath, mtllib), mtexes)], msize)



def guess_model_dependencies(model: str, texture: str, asset: str) -> ModelInfo:
    """
    Guesses the dependency models, or returns the default model and
    texture combination
    """
    mname, mext = os.path.splitext(model)
    logging.info(f"guessing {args.model}")

    handlers = {
            ".obj": guess_obj_file,
            ".md2": guess_without_texture_info
    }

    fn = handlers.get(mext.lower())
    if fn is None:
        raise RuntimeError(f"{model} is not a supported file!")

    return fn(model, texture, asset)



def print_model_information(m: ModelInfo) -> str:
    mdict = {
            "name": m.model_name,
            "path": m.model_path,
            "type": "mesh"
    }

    matdicts = []
    texdicts = []

    if len(m.material) > 0:
        mat = m.material[0]
        if len(mat.textures) > 0:
            mdict["mesh.texture"] = mat.textures[0].name

            texdicts.append({
                "name": mat.textures[0].name,
                "path": mat.textures[0].path,
                "type": "texture"
            })

        if mat.name is not None:
            mdict["mesh.material"] = mat.name
            
            md = {
                "name": mat.name,
                "path": mat.path,
                "type": "material",
            }

            if len(mat.textures) > 0:
                md["texture"] = mat.textures[0].name

            matdicts.append(md)

    logging.debug(repr(texdicts))

    assetdict = [mdict]
    assetdict.extend(matdicts)
    assetdict.extend(texdicts)

    v = yaml.dump({"assets": assetdict})

    if m.size is not None:
        x, y, z = m.size
        v += f"\n# size: {x:.3f} x {y:.3f} x {z:.3f}\n"

    # We dump a text that can be appended into the current YAML, so we replace the first line
    return v.replace("assets:", f"# asset data for model {m.model_name}\n", 1)


logging.basicConfig(level=logging.DEBUG)

parser = argparse.ArgumentParser(description='Guesses asset dependencies and write a YAML description of them')
parser.add_argument("model", type=str, help="The name of the model")
parser.add_argument("--texture", 
                    help="an optional texture, for models that does not embed one", 
                    default=None)
parser.add_argument("--no-material",
                    action='store_true',
                    help="do not use the embedded material")
parser.add_argument("--asset", 
        help="The asset model name, Defaults to the filename if not specified", 
        default=None)


args = parser.parse_args()
asset = args.asset if args.asset is not None else get_asset_name_from_path(args.model)

try:
    minfo = guess_model_dependencies(args.model, args.texture, asset)
    if args.no_material:
        for m in minfo.material:
            # doing this way, we do not mess with the texture
            m.name = None
            m.path = None
            
    print(print_model_information(minfo))
except RuntimeError as e:
    print(f"Error: {e.args[0]}", file=sys.stderr)

