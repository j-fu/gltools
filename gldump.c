void glwDumpOnSwap(glWindow w)
{
  if (w->mpeg_recording && (w->mpeg_frame_num%w->mpeg_skip==0))
    {
      glwGrabPPM(w,w->mpeg_file);
      w->mpeg_recorded_frame_num++;
    }
  w->mpeg_frame_num++;

  if (w->eps_recording && (w->eps_frame_num%w->eps_skip==0))
    {
      if(w->eps_recorded_frame_num==0)
	glepsEmitHeader(w,w->eps_file); /* emitting header here solves
					       clear color problem */
      glepsGrabEPSFrame(w,w->eps_file,w->eps_recorded_frame_num+1);
      w->eps_recorded_frame_num++;
    }
  w->eps_frame_num++;
}

/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
/*----------------------------------------------------------------*/
void glwStartEPSRecording(
		       glWindow win, 
		       char *fileName,
		       int skip
		       )
{
  if (strcmp(strrchr(fileName,'.'),".pdf")==0) 
    {
      static char cmdbuf[128];
      sprintf(cmdbuf,"ps2pdf - %s",fileName);
      win->eps_file=popen(cmdbuf,"w");
      win->eps_recording=2;
    }
  else
    {
      win->eps_file=fopen(fileName,"w");
      win->eps_recording=1;
    }
  assert(win->eps_file&&"unable to open eps/pdf file");
  win->eps_recorded_frame_num=0;
  win->eps_frame_num=0;
  if (skip<0) skip=0;
  win->eps_skip=skip+1;
}

void glwStopEPSRecording(glWindow win)
{
  glepsEmitTrailer(win,win->eps_file,win->eps_recorded_frame_num);

  if (win->eps_recording==1)
    {
      fclose(win->eps_file);
      printf("%d eps frames recorded\n",win->eps_recorded_frame_num);
    }
  else   if (win->eps_recording==2)
    {
      pclose(win->eps_file);
      printf("%d pdf frames recorded\n",win->eps_recorded_frame_num);
    }
  win->eps_recording=0;
}
 
void glwStartMPEGRecording(
		       glWindow win, 
		       char *fileNameStub,
		       int skip
		       )
{
  FILE *param_file=NULL;
  FILE *user_param_file=NULL;
  char *user_param_file_name;
  int w,h;
  user_param_file_name=glwGetDefault(win,"mpegParams",NULL);
  if (user_param_file_name!=NULL) 
    {
      user_param_file=fopen(user_param_file_name,"r");
      if (user_param_file==NULL)
	glwError("user parameter file not found");
    }
  param_file=fopen(".glwin.mpeg.param","w");
  if (user_param_file==NULL) 
    /* set own default if user
       parameter file not given */
    {
      fprintf(param_file,"PATTERN  IBBPBBPBBPBBPBB\n");
      fprintf(param_file,"GOP_SIZE  30\n");
      fprintf(param_file,"SLICES_PER_FRAME 1\n");
      fprintf(param_file,"PIXEL	HALF\n");
      fprintf(param_file,"RANGE	10\n");
      fprintf(param_file,"PSEARCH_ALG	LOGARITHMIC\n");
      fprintf(param_file,"BSEARCH_ALG	CROSS2\n");
      fprintf(param_file,"IQSCALE 8\n");
      fprintf(param_file,"PQSCALE 10\n");
      fprintf(param_file,"BQSCALE  25\n");
      fprintf(param_file,"REFERENCE_FRAME	ORIGINAL\n");
    }
  else
    {
      while (!feof(user_param_file))
	{
	  fgets(glw_string_buffer,GLW_BUFSIZE,user_param_file);
	  fputs(glw_string_buffer,param_file);
	}
      fclose(user_param_file);
  }
  glwGetWindowSize(win,&w,&h);
  
  fprintf(param_file,"BASE_FILE_FORMAT	PNM\n");
  fprintf(param_file,"INPUT_DIR stdin\n");
  fprintf(param_file,"INPUT_CONVERT *\n");
  fprintf(param_file,"INPUT\n");
  fprintf(param_file,"END_INPUT\n");
  fprintf(param_file,"PNM_SIZE %dx%d\n",w,h);
  fprintf(param_file,"OUTPUT %s.mpg\n",fileNameStub);
  fclose(param_file);
  win->mpeg_file=popen("mpeg_encode .glwin.mpeg.param","w");
  win->mpeg_recorded_frame_num=0;
  win->mpeg_frame_num=0;
  win->mpeg_recording=1;
  if (skip<0) skip=0;
  win->mpeg_skip=skip+1;
}

void glwStopMPEGRecording(glWindow win)
{
  win->mpeg_recording=0;
  pclose(win->mpeg_file);
  printf("%d mpeg frames recorded\n",win->mpeg_recorded_frame_num);
}


#define GLW_SCREEN_WIDTH 2048
char pixelbuf[GLW_SCREEN_WIDTH*3];

void glwGrabPPM(glWindow win, FILE *f)
{
  register int i;
  int w,h;
  glwGetWindowSize(win,&w,&h);
  assert(f!=NULL);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_SWAP_BYTES, GL_TRUE);
  fprintf(f, "P6\n");
  fprintf(f, "%d %d\n", w, h);
  fprintf(f, "%d\n", 255);
  for(i=1;i<=h;i++)
    {
      glReadPixels(0, h-i, w,1, GL_RGB, GL_UNSIGNED_BYTE, pixelbuf);
      fwrite(pixelbuf, 3, w, f);
    }
}

void  glwGrabPPMAndPrint(glWindow w)
{
  FILE *f;
  char *command;
  command=glwGetDefault(w,"printerCommand","pnmtops | lpr");
  f=popen(command,"w");
  glwGrabPPM(w,f);
  pclose(f);
}

void  glwDump(
              glWindow win, 
              void * info, 
              char *fileName, 
              int w, 
              int h
              )
{
  FILE *f=fopen(fileName,"w");
  glwGrabPPM(win,f);
  fclose(f);
}


/*******************************************/

void glwRenderFB(glWindow w)
{

  int size=w->fb_initial_size;
  w->fb_size=0;
  while (w->fb_size<=0)
    { 
      w->fb_buffer =(GLfloat*) calloc(size, sizeof(GLfloat));
      if (w->fb_buffer==NULL)
	{
	  glwError("Not enough memory for feedback buffer");
	  return;
	}
      glFeedbackBuffer(size, GL_3D_COLOR, w->fb_buffer);
      glRenderMode(GL_FEEDBACK);
      w->fb_mode=1;
      (*w->RedrawFunc)(w,w->info);
      w->fb_mode=0;
      w->fb_size = glRenderMode(GL_RENDER);
      if (w->fb_size<=0)
	{
	  glwResetFB(w);
	  fprintf(stderr,"resizig feedback buffer to %.3g MB\n", ((double)size)/(1024.0*1024.0));
	  size=2*size;
	}
    }
  w->fb_initial_size=size;
}

void glwResetFB(glWindow w)
{
  int i;
  for (i=0;i<w->fb_num_strings;i++)
    {  
      if (w->fb_strings[i])
	free(w->fb_strings[i]);
      w->fb_strings[i]=NULL;
    }
  w->fb_num_strings=0;  
  w->fb_size=0;
  if (w->fb_buffer!=NULL) free(w->fb_buffer);
}

float *glwGetFB(glWindow w)
{
  return w->fb_buffer;
}
int glwGetFBSize(glWindow w)
{
  return w->fb_size;
}

char *glwGetFBString(glWindow w,int istring)
{
  return w->fb_strings[istring];
}
double glwGetFBStringSize(glWindow w,int istring)
{
  return w->fb_string_size[istring];
}

int glwPrintFB(glWindow w, char *s)
{
  if (w->fb_mode)
    {
      GLint buf=0;
      if (w->fb_num_strings<GLW_FEEDBACK_MAX_STRINGS)
	{
	  glDrawPixels(1,1,GL_COLOR_INDEX,GL_INT,&buf); /* draw some cheap dummy */
	  glPassThrough(w->fb_num_strings);
	  w->fb_strings[w->fb_num_strings]=strdup(s);
	  w->fb_string_size[w->fb_num_strings]=w->font_size;
	  w->fb_num_strings++;
	}
      return 1;
    }
  else
    return 0;
}

void glwInitFB(glWindow w)
{
  w->fb_num_strings=0;
  w->fb_buffer=0;
  w->fb_initial_size=1*1024*1024; /*  MB */
}


/*
$Log: gldump.c,v $
Revision 2.3  2001/03/08 17:37:28  fuhrmann
First running version of gltensview stuff

Revision 2.2  2001/03/02 16:38:04  fuhrmann
Arbeiten fuer Fuelcell/Juelich

*/

