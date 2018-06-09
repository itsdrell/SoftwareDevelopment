<shader name="Sprite">
<program>
      <both file="Data/Shaders/glsl/spriteShader"/>
   </program>

   <depth write="true" test="less"/>

    <!-- Material Defaults -->
     <Texture path = "Images/defaultTexture.png" sampler = "default" />
</shader>