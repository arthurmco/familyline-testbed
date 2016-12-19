package net.arthurmco.Tribalia.assetmanager;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

public class AssetLoader {

    /**
     * Gets a file name and discover what type of asset it is
     * @param assetname The asset name, only for returning with the asset
     * @param filename The file you wish to get.
     * @return The asset object, or null if no compatible asset were found.
     */
    public static Asset Load(String assetname, String filename) {
        
        /*  For now, we separate by extension.
            But, for further extensivity, we should open the file and try
            to discover what type it is.
            
        */
        
        String ext = filename.substring(filename.lastIndexOf(".")+1);
        
        switch (ext) {
            case "obj": 
            case "md2":
                Mesh m = new Mesh(assetname, filename);
                return m;
                
            case "png":
            case "bmp":
                Texture t = new Texture(assetname, filename);
                return t;
                
            case "mtl":
                Material mt = new Material(assetname, filename);
                return mt;
                
                
        }
        
        return null;
    }
    
}
