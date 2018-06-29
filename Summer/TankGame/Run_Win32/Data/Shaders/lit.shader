<shader name="lit">
   <program define="USE_DIFFUSE;USE_SPECULAR;FOG">
      <vertex file="Data/Shaders/glsl/mvp_lit.vs" />
      <fragment file="Data/Shaders/glsl/lit.fs" />
   </program>

    <Property type = "specular" name = "attributeName" value = "1,20" />

</shader>