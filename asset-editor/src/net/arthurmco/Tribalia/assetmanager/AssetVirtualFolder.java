/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package net.arthurmco.Tribalia.assetmanager;

import java.util.ArrayList;

/**
 *
 * @author arthurmco
 */
public class AssetVirtualFolder extends Asset {

    private ArrayList<Asset> children;
    public AssetVirtualFolder(String name) {
        super(name, Asset.TYPE_FOLDER);
        
        children = new ArrayList<>();
    }
    
    public void add(Asset a) {
        children.add(a);
    }
    
    public void remove(Asset a) {
        children.remove(a);
    }
    
    public boolean contains(Asset a) {
        return children.contains(a);
    }
    
    public Asset getByName(String name) {
        String basename = "";
        
        if (name.indexOf("/") > 0) {
            basename = name.substring(0, name.indexOf("/"));
        }
        
        /* Get an asset by its name */
        for (Asset a : children) {
            if (a.getName().equals(name)) {
                return a;
            }
            
            /*  If we find the base name only, and not the full, treat it like
                a folder, and find its contents
            */
            if (a.getName().equals(basename)) {
                if (a instanceof AssetVirtualFolder) {
                    AssetVirtualFolder avf = (AssetVirtualFolder) a;
                    return avf.getByName(name.substring(basename.length()));
                }
            }
            
        }
        
        return null;
    }
    
}
