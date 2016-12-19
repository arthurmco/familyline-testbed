/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package net.arthurmco.Tribalia.assetmanager;

/**
 * Mesh asset class
 */
public class Mesh extends Asset {
    private String file;

    public Mesh(String name, String file) {
        super(name, Asset.TYPE_MESH);
        this.file = file;
    }

    public String getFile() {
        return file;
    }

    public void setFile(String file) {
        this.file = file;
    }
        
}
