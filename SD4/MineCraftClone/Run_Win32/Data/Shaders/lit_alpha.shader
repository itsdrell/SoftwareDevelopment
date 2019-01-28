<!-- If there are options for an attribute, the first is always the default to 
     be used if no option is given. -->

<shader name="lit_alpha">
   <program define="USE_DIFFUSE;USE_SPECULAR">
      <vertex file="Data/Shaders/glsl/mvp_lit.vs" />
      <fragment file="Data/Shaders/glsl/lit.fs" />
   </program>

   <blend>
      <color op="add"
         src="src_alpha"
         dest="inv_src_alpha" />
   </blend>

   <!-- compare less is default -->
   <depth write="false" />
</shader>