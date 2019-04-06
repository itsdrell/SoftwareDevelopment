<shader name="defaultWireframe" frontface="cw" cull="back" fill="wire"> 
    <program define="Hello;Fam">
      <both file="Data/Shaders/glsl/Default"/>
   </program>

   <depth write="true" test="less"/>

</shader>




<!--
If there are options for an attribute, the first is always the default to 
be used if no option is given. 

<shader name="name" sort="0" cull="back|front|none" fill="solid|wire" frontface="ccw|cw">
	<program id="opt_program_id" define="semicolon;seperated;list">
	<!--  if no id, you can define the program here manually  -->
	<!--  id and defines are mutually exclusive  -->
		<vertex file="shader/glsl/mvp_lit.vs"/>
		<fragment file="shader/glsl/lit.fs"/>
	</program>

	<blend>
		<color op="add|sub|rev_sub|min|max" src="one|zero|src_color|inv_src_color|src_alpha|inv_src_alpha|dest_color|inv_dest_color|dest_alpha|inv_dest_alpha|constant|inv_constant" dest="zero"/>
		<alpha op="add|sub|rev_sub|min|max" src="one" dest="one"/>
	</blend>
	
	<depth write="true|false" test="less|never|equal|lequal|greater|gequal|not|always"/>

	<!-- These are for materials, they are optional, but good to set defaults for this shader so 
		it isn't using bad Data
	-->
	
	<Textures>
        <Texture
        path = "fileName.ext"
        bind = "index"
        sampler = "default | more"
        />
    </Textures>

    <Properties>
        <Property 
        type = "float| vec4 | color | specular | etc"
        name = "attributeName" 
        value = "basedOffTheType"
        />
    </Properties>


</shader>

-->