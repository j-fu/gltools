#define GLW_FEEDBACK_BUFSIZE 1000000
#define GLW_FEEDBACK_MAX_STRINGS 1000

  int  mpeg_recording;
  int  mpeg_recorded_frame_num;
  int mpeg_frame_num;
  FILE *mpeg_file;
  int mpeg_skip;

  int  eps_recording;
  int  eps_recorded_frame_num;
  int eps_frame_num;
  FILE *eps_file;
  int eps_skip;



  int fb_mode;
  int fb_num_strings;
  char *fb_strings[GLW_FEEDBACK_MAX_STRINGS];
  double fb_string_size[GLW_FEEDBACK_MAX_STRINGS];
  GLfloat *fb_buffer;
  int fb_initial_size;
  int fb_size;
