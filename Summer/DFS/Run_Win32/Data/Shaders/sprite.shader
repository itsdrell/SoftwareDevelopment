<shader name="Sprite">
<program>
      <both file="Data/Shaders/glsl/spriteShader"/>
   </program>

   <depth write="true" test="always"/>

    <!-- Material Defaults -->
     <Texture path = "Images/defaultTexture.png" sampler = "default" />

     <Property type = "color" name = "TINT" value = "255,255,255,255"/>
     
</shader>