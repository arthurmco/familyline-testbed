/***
    Material manager

    Copyright (C) 2016 Arthur Mendes.

***/

#include <vector>
#include <cstring> //strcmp()
#include "Material.hpp"
#include "Log.hpp"

#ifndef MATERIALMANAGER_HPP
#define MATERIALMANAGER_HPP

namespace Tribalia {
namespace Graphics {

    class MaterialManager
    {
    private:
        std::vector<Material> _materials;

        static MaterialManager* _mm;

    public:
        /* Add material, return its ID */
        int AddMaterial(Material*);

        /* Add materials*/
        void AddMaterials(std::vector<Material*>);

        Material* GetMaterial(int ID);
        Material* GetMaterial(const char* name);

        static MaterialManager* GetInstance()
        {
            if (!_mm)
                _mm = new MaterialManager();

            return _mm;
        }
    };


}
}


#endif /* end of include guard: MATERIALMANAGER_HPP */
