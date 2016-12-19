/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package net.arthurmco.Tribalia.assetmanager;

/**
 * Material asset class
 */
public class Material extends Asset {
    private String file;

    public Material(String name, String file) {
        super(name, Asset.TYPE_MATERIAL);
        this.file = file;
    }

    public String getFile() {
        return file;
    }

    public void setFile(String file) {
        this.file = file;
    }
    
    
}
