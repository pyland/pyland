#include "MAP_H"

/**
 * Constructor for Map
 */ 
Map::Map() {

}

/** 
 *Destructor for Map
 */
Map::~Map() {
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   
   // release buffers
   delete []tex_buf1;
   delete []mapData;
   delete []sprite_data;
   delete []sprite_tex_data;
   delete []mapTexCoords;
   delete []tileSetTexCoords;

   printf("\nClosed\n");

}
/**
 * Function used to generate the necessary Vertex Buffer Objects to
 * hold the map data to achieve more efficient rendering.
 */
void Map::init_vbo_buffer() {
  glUseProgram(program_obj);

  //vboIds[0] = map geometric data
  //vboIds[1] = tileset
  //vboIds[2] = sprite geometric data
  //vboIds[3] = texture coords for sprite
  glGenBuffers(num_vbo_ids, vboIds);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
  glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*18, mapData, GL_STATIC_DRAW);
  

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
  glBufferData(GL_ARRAY_BUFFER, map_height*map_width*sizeof(GLfloat)*12, mapTexCoords, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*18, sprite_data, GL_STATIC_DRAW);

  //changing texture coords
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, sprite_tex_data, GL_DYNAMIC_DRAW);

  //MAPS
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
  glEnableVertexAttribArray(VERTEX_POS_INDX);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
  glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);


  //SPRITES
  glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);
  glEnableVertexAttribArray(VERTEX_POS_INDX);

  glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);
  glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);



  glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE,0, 0);
  glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,texture_id);
}

/**
 * The function used to generate the cache of tile texture coordinates.
 */ 
void Map::generate_tileset_coords(int tileset_width, int tileset_height) {

 #ifdef DEBUG
   printf("GENERATING TILESET TEXTURE COORDS...");
 #endif
   //check the tilset image height and widths are multiples of the tiles
   //  assert(image_height % TILESET_ELEMENT_SIZE != 0 || image_width % TILESET_ELEMENT_SIZE != 0);


   int numTilesX = image_width/ TILESET_ELEMENT_SIZE;
   int numTilesY = image_height / TILESET_ELEMENT_SIZE;

   //Each tile needs 8 floats to describe its position in the image
   tileSetTexCoords = new GLfloat[sizeof(GLfloat)* numTilesX * numTilesY * 4 * 2];
   assert(tileSetTexCoords != 0);
   //TODO free this memory

     double tileSetXOffset = 0.0;
  double tileSetYOffset = 0.0;
  double tileSetXInc = 1.0 / (double)numTilesX;
  double tileSetYInc = 1.0 / (double)numTilesY;
  //TODo: DIV ZEro HERRE 

  //TODO: REMEMBER TILESET COORDINATES ARE INVERSE OF IMAGE FILE ONES
  //generate the coordinates for each tile
  for(int x = 0; x < numTilesX; x++)
    {
      for(int y = 0; y< numTilesY; y++)
	{
	  //bottom left
	  tileSetTexCoords[x* numTilesY*4*2+y*(4*2)] = tileSetXOffset;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2 +1] =tileSetYOffset;

	  //top left
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+ 2] =tileSetXOffset;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+3] = tileSetYOffset + tileSetYInc;

	  //bottom right
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+4] = tileSetXOffset + tileSetXInc;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+5] = tileSetYOffset;

	  //top right
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+6] = tileSetXOffset + tileSetXInc;
	  tileSetTexCoords[x* numTilesY*4*2+y*4*2+7] = tileSetYOffset + tileSetYInc;

	  tileSetYOffset += tileSetYInc;
	}
      tileSetXOffset += tileSetXInc;
      tileSetYOffset = 0.0;
    }

}

/**
 * The function which generates the texture coordinates for the map
 * geometry, using the cached tile coordinates.
 */
void Map:: generate_map_texcoords() {

#ifdef DEBUG
  printf("GENERATING MAP TEXTURE DATA...");
#endif
  //holds the map data
  //need 12 float for the 2D texture coordinates
  int num_floats = 12;
  mapTexCoords = new GLfloat[sizeof(GLfloat)*map_height*map_width*num_floats]; 
  assert(mapTexCoords != 0);

  int x, y;

  //generate the map data
  // get the tile set coordinates for the particular tile
  for(x = 0; x < map_width; x++)
    {
      for(y = 0; y < map_height; y++)
	{
	  int currTile = worldData[x*map_height +y];
	  GLfloat *tileSetPtr = &tileSetTexCoords[currTile*8];
	  //bottom left
	  mapTexCoords[x*map_height*num_floats + y*num_floats+0] = tileSetPtr[0];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+1] = tileSetPtr[1];

	  //top left
	  mapTexCoords[x*map_height*num_floats + y*num_floats+2] = tileSetPtr[2];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+3] = tileSetPtr[3];

	  //bottom right
	  mapTexCoords[x*map_height*num_floats + y*num_floats+4] = tileSetPtr[4];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+5] = tileSetPtr[5];

	  //top left
	  mapTexCoords[x*map_height*num_floats + y*num_floats+6] = tileSetPtr[2];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+7] = tileSetPtr[3];

	  //top right
	  mapTexCoords[x*map_height*num_floats + y*num_floats+8] = tileSetPtr[6];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+9] = tileSetPtr[7];
	
	  //bottom right
	  mapTexCoords[x*map_height*num_floats + y*num_floats+10] = tileSetPtr[4];
	  mapTexCoords[x*map_height*num_floats + y*num_floats+11] = tileSetPtr[5];

 	}
    }
}

/*
 * The function which generates the map geometry so that it can be
 * rendered to the screen
 */
void Map::generate_map_coords() {
#ifdef DEBUG
  printf("GENERATING MAP DATA...");
#endif
  //holds the map data
   //need 18 floats for each coordinate as these hold 3D coordinates
  int num_floats = 18;
  mapData = new GLfloat[sizeof(GLfloat)*map_height*map_width*num_floats]; 
  assert(mapData != 0);
  float scale = 16.0f;
  //generate the map data
  /**
   * Vertex winding order:
   * 1, 3   4
   *  * --- * 
   *  |     |
   *  |     | 
   *  * --- *
   * 0       2,5
   */
  int x, y;
  for(x = 0; x < map_width; x++)
    {
      for(y = 0; y < map_height; y++)
	{
	  //generate one tile's worth of data

	  
	  //bottom left
	  mapData[x*map_height*num_floats + y*num_floats+0] = x * scale;
	  mapData[x*map_height*num_floats + y*num_floats+1] = y * scale;
	  mapData[x*map_height*num_floats + y*num_floats+2] = 0;
	   
	  //top left
	  mapData[x*map_height*num_floats + y*num_floats+3] = x * scale;
	  mapData[x*map_height*num_floats + y*num_floats+4] = (y+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+5] = 0;

	  //bottom right
	  mapData[x*map_height*num_floats + y*num_floats+6] = (x+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+7] = y * scale;
	  mapData[x*map_height*num_floats + y*num_floats+8] = 0;
	
	  //top left
	  mapData[x*map_height*num_floats + y*num_floats+9] = x * scale;
	  mapData[x*map_height*num_floats + y*num_floats+10] = (y+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+11] = 0;
  
   	  //top right
	  mapData[x*map_height*num_floats + y*num_floats+12] = (x+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+13] = (y+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+14] = 0;

	  //bottom right
	  mapData[x*map_height*num_floats + y*num_floats+15] = (x+1) * scale;
	  mapData[x*map_height*num_floats + y*num_floats+16] = y * scale;
	  mapData[x*map_height*num_floats + y*num_floats+17] = 0;
	}
    }
#ifdef DEBUG
  printf("DONE.");
#endif

}

/**
 * This function loads the shaders
 */ 
GLuint Map::load_shader(GLenum type, const std::string src) {
  GLuint shader;
  GLint compiled = 0;
  
  //Create the shader object 
  shader = glCreateShader(type);

  if(shader == 0) 
    return 0; //couldn't create the shader

  //Load shader source code
  const char* source = src.c_str();
  glShaderSource(shader, 1, &source, nullptr);

  //Compile the shader
  glCompileShader(shader);

  //Check for errors
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  //Handle the errors
  if(!compiled) {
    GLint info_len = 0;
    
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
    
    if(info_len > 1) {
      char* info_log = new char[sizeof(char) * info_len];

      glGetShaderInfoLog(shader, info_len, nullptr, info_log);
      std::cerr << "ERROR: SHADER LOADING " << std::endl  << info_log << std::endl;
      delete []info_log;
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;

} 

/** 
 * This function crates the Opengl program
 */
GLuint Map::shader_create(const string vs, const string fs) {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLint linked;

    //Load the fragment and vertex shaders
    vertex_shader = load_shader(GL_VERTEX_SHADER, vs);
    fragment_shader = load_shader(GL_FRAGMENT_SHADER, fs);

    //Create the program object
    program_obj = glCreateProgram();

    if(program_obj == 0) {
      cerr << "ERROR FLAG: " << glGetError();
      std::cerr << "ERROR: SHADER PROGRAM CREATION. Could not create program object." << std::endl;
      return 0;
    }
    cout << "PROGRAM: " << program_obj << endl;
    glAttachShader(program_obj, vertex_shader);
    glAttachShader(program_obj, fragment_shader);


    glBindAttribLocation(program_obj, VERTEX_POS_INDX, "a_position");
    glBindAttribLocation(program_obj, VERTEX_TEXCOORD0_INDX, "a_texCoord");

    //Link the program
    glLinkProgram(program_obj);

    //Check to see if we have any log info
    glGetProgramiv(program_obj, GL_LINK_STATUS, &linked);
    
    if(!linked) {
      GLint info_len = 0;
      
      glGetProgramiv(program_obj, GL_INFO_LOG_LENGTH, &info_len);

      if(info_len > 1) {
        char* info_log = new char[sizeof(char)*info_len];

        glGetProgramInfoLog(program_obj, info_len, nullptr, info_log);
        std::cerr << "ERROR: PROGRAM LINKING " << std::endl  << info_log << std::endl;
        delete []info_log;
      }
      glDeleteProgram(program_obj);
      return 0;
    }
    
    return program_obj;
}

/** 
 * This function loads the required texture images
 */ 
void Map::load_tex_images() {
    FILE *tex_file1, *tex_file2 = nullptr;
    size_t bytes_read, image_sz = IMAGE_SIZE_WIDTH*IMAGE_SIZE_HEIGHT*3;

    tex_buf1 = new char[image_sz];

    tex_file1 = fopen(PATH "../graphics/tiles/Djenne_128_128.raw", "rb");
    if (!tex_file1) {
        std::cerr << "ERROR: Couldn't load textures" << endl;
    }

    if (tex_file1 && tex_buf1) {
        bytes_read = fread(tex_buf1, 1, image_sz, tex_file1);
        assert(bytes_read == image_sz);  // some problem with file?
        fclose(tex_file1);
    }

    // tex_buf2 = new char[image_sz];

    // tex_file2 = fopen(PATH "../graphics/tiles/assets/characters_1.raw", "rb");
    // if (!tex_file1) {
    //   std::cerr << "ERROR: Couldn't load textures" << endl;
    // }

    // if (tex_file1 && tex_buf1)
    // {
    //    bytes_read=fread(tex_buf1, 1, image_sz, tex_file1);
    //    assert(bytes_read == image_sz);  // some problem with file?
    //    fclose(tex_file1);
    // }
}

/** 
 * This function performs the required Opengl initializatin for the textures.
 */ 
void Map::init_textures() {
  
  //  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  load_tex_images();
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_SIZE_WIDTH, IMAGE_SIZE_HEIGHT, 0,
                GL_RGB, GL_UNSIGNED_BYTE, tex_buf1);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)GL_NEAREST);

}

/**
 * This function initialises the shader, creating and loading them.
 */ 
bool Map::init_shaders() {
#ifdef USE_GLES
  //read in the shaders
  ifstream vertex_shader_src("vert_shader.glesv");
  ifstream fragment_shader_src("frag_shader.glesf");
#endif
#ifdef USE_GL
  //read in the shaders
  ifstream vertex_shader_src("vert_shader.glv");
  ifstream fragment_shader_src("frag_shader.glf");

#endif
  if(!vertex_shader_src.good()){
    std::cerr << "Failed to load vertex shader" << endl;
    return false;
  }
    
  if(!fragment_shader_src.good() ){
    cerr << "Failed to load fragment shader" << endl;
    return false;
  }

  std::string vert_src, frag_src, line;
  while(getline(vertex_shader_src, line)) 
    vert_src += line + "\n";

  while(getline(fragment_shader_src, line))
    frag_src += line + "\n";

  GLuint program_obj = shader_create(vert_src, frag_src);

  if(program_obj == 0){
    std::cout << "Failed to create the shader" << std::endl;
    return false;
  }

  return true;

}

/**
 * The function used to render the map. Makes the necessary Opengl
 * to correctly render the map.
 */
void Map::render_map() {

   //LEAVE THIS HERE!
   glDisable(GL_CULL_FACE);
   glm::mat4 model = glm::mat4(1.0f);
   glm::mat4 translated = glm::translate(model, glm::vec3(map_bottom_x, map_bottom_y, 0.0f));

   glUseProgram(program_obj);

   glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_projection"), 1, GL_FALSE,glm::value_ptr(projection_matrix));
   glUniformMatrix4fv(glGetUniformLocation(program_obj, "mat_modelview"), 1, GL_FALSE, glm::value_ptr(translated));


   glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
   glVertexAttribPointer(VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(VERTEX_POS_INDX);


   glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);
   glVertexAttribPointer(VERTEX_TEXCOORD0_INDX, 2, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(VERTEX_TEXCOORD0_INDX);

   glBindAttribLocation(program_obj, VERTEX_POS_INDX, "a_position");


   glBindAttribLocation(program_obj, VERTEX_TEXCOORD0_INDX, "a_texCoord");

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture_id);

   //set sampler texture to unit 0
   glUniform1i(glGetUniformLocation(program_obj, "s_texture"), 0);


   glDrawArrays(GL_TRIANGLES, 0, 6*map_width*map_height);
   glUseProgram(0);

}

/**
 * The function used to update elements on the map.
 */
void update_map(float dt) {

}
