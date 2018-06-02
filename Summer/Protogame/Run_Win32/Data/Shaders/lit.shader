<shader name="lit">
   <program define="USE_DIFFUSE;USE_SPECULAR">
      <vertex file="Data/Shaders/glsl/mvp_lit.vs" />
      <fragment file="Data/Shaders/glsl/lit.fs" />
   </program>

   <!-- Material Defaults -->
    <Texture path = "Images/couch/example_colour.png" sampler = "default"/>
    <Texture path = "Images/couch/example_normal.png" sampler = "default"/>
    <Texture path = "Images/couch/SciFi_Fighter-MK6-lights.jpg" sampler = "default"/>

    <Property type = "specular" name = "attributeName" value = "1,20" />

</shader>