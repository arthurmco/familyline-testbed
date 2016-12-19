/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package net.arthurmco.Tribalia.assetmanager;

/**
 * Basic asset class
 */
public class Asset {
    private String name;
    private String type;
    
    /* Define constant type strings */
    public static final String TYPE_MESH = "mesh";
    public static final String TYPE_TEXTURE = "texture";
    public static final String TYPE_MATERIAL = "material";
    public static final String TYPE_FOLDER = "folder";

    public Asset(String name, String type) {
        this.name = name;
        this.type = type;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getType() {
        return type;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj instanceof Asset) {
            Asset a = (Asset) obj;
            return (a.name.equals(this.name) && a.type.equals(this.type));
        }
        return false;
    }

    @Override
    public String toString() {
        return name;
    }
    
    
    
    
    
}
