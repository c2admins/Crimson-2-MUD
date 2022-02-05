  
/******************************************************************** *
 *  file: rebuildeqp.c, Player EQP rebuild module. Part of DIKUMUD    *
 *  Usage:  rebuildeqp -u -v                                          *
 *********************************************************************/
  
/* CHANGE THE FOLLOWING IF SIZES CHANGE */
/* YOU CAN PUT THE OLD STRUCTURES IN    */
/* THE rebuildeqp.h FILE                */
#define OLD_HEAD obj_file_u
#define NEW_HEAD obj_file_u
#define OLD_ELEM old_obj_file_elem
#define NEW_ELEM obj_file_elem

  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>
  #include <time.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <errno.h>
  
  #include "structs.h"
  #include "utility.h"
  #include "db.h"
  #include "comm.h"
  #include "handler.h"
  #include "limits.h"
  #include "constants.h"
  #include "ban.h"
  #include "weather.h"
  #include "globals.h"
  #include "rebuildeqp.h"

  #define SAVE_ALIAS 1
  #define LOAD_ALIAS 2

sh_int convert_short(int numin);
int    convert_int(int numin);
int    old_rent_get_filename(char *orig_name, char *filename);
int    new_rent_get_filename(char *orig_name, char *filename);
int    eqp500_alias_get_filename(
              char *orig_name, char *filename, int lv_direction);
void   eqp9800_load_save_aliases
              (struct char_data *ch, int lv_direction);

int gv_convert_flag;
char lv_dir_in[] = {"/prod/lib"};
char lv_dir_out[] = {"/devel/lib"};

main (int argc, char **argv) {

    FILE *file_in,
         *file_out,
         *file_player;

    int verbose_print = FALSE,
        create_new_file = FALSE,
        add_hp,
        lv_level,
        idx;

    time_t t;
    int size_of_player_record,
        size_of_old_head,
        size_of_old_elem,
        size_of_new_head,
        size_of_new_elem,
        lv_plr_loop,
        lv_eqp_loop,
        number_written,
        pos,
        rc,
        i, j;
   
    char *ptr,
         buf[MAX_STRING_LENGTH],
         filename[256];
 
    struct char_data         gv_ch;
    struct char_file_u       new_ch;
    struct OLD_HEAD          old_head;
    struct OLD_ELEM          old_elem;
    struct NEW_HEAD          new_head;
    struct NEW_ELEM          new_elem;

    size_of_player_record = sizeof(struct char_file_u);
    size_of_old_head      = sizeof(struct OLD_HEAD);
    size_of_old_elem      = sizeof(struct OLD_ELEM);
    size_of_new_head      = sizeof(struct NEW_HEAD);
    size_of_new_elem      = sizeof(struct NEW_ELEM);

    printf("Use -c for integer conversion.\r\n");
    printf("Use -v for verbose.\r\n");
    printf("Use -u for update.\r\n");
  
    gv_convert_flag == FALSE;
    number_written = 0;
    pos = 1;
    while ((pos < argc) && (*(argv[pos]) == '-')) {
        switch (*(argv[pos] + 1)) {
            case 'c' : case 'C' :
                  gv_convert_flag = TRUE;
                  break;
            case 'u' : case 'U' :
                  create_new_file = TRUE;
                  break;
            case 'v' : case 'V' :
                  verbose_print = TRUE;
                  break;
            default :
                  break;
        }
        pos++;
    } 

    sprintf(filename, "%s/players", lv_dir_in);
    printf("Opening input database: %s\r\n", filename);
    file_player = fopen(filename, "rb"); 
    if (!file_player) {
       bzero(buf, sizeof(buf));
       sprintf(buf, "ERROR: Unable to open player file %d.\r\n",
                       errno);
       printf(buf);
       exit(1);
    }

    t = time(NULL);
    printf("Running rebuild at: %s\r\n", ctime(&t));
  
    /* MOVE STUFF FROM OLD STRUCT TO NEW STRUCT */
    ptr = (char *)&new_ch;
    bzero(ptr, sizeof(new_ch));

    fread(&new_ch, size_of_player_record, 1, file_player);
    for (lv_plr_loop=0; (!feof(file_player)); lv_plr_loop++) {
       if (*new_ch.name==0     || /* THROW OUT NULLS   */
           *new_ch.name == 32  || /* THROW OUT BLANKS  */
           *new_ch.name == '1' || /* THROW OUT DELETES */
           *new_ch.name == '2' || /* THROW OUT SELF DELETS */
           /* lv_plr_loop == 1 || */
           lv_plr_loop > 10000) {
               if (verbose_print == TRUE) { 
                   printf("Skipping %d name= %s\r\n",
                                   lv_plr_loop, new_ch.name);
               }
       }
       else {
          /* CONVERT FIRST LETTER TO LOWERCASE */
          if (new_ch.name[0] >= 'a' && new_ch.name[0] <= 'z')
                       new_ch.name[0] - 32;

          if (verbose_print == TRUE) { 
            printf("\r\n    *************************************\r\n");
            printf("%d %d %15s %s\r\n", 
                        lv_plr_loop, new_ch.level, new_ch.name, new_ch.title);
          } /* END OF DO PRINT */             

          /* COPY ALIAS FILE */
          gv_ch.player.name = malloc(strlen(new_ch.name) + 5);
          strcpy(gv_ch.player.name, new_ch.name);
          eqp9800_load_save_aliases(&gv_ch, LOAD_ALIAS);
          eqp9800_load_save_aliases(&gv_ch, SAVE_ALIAS);
          free(gv_ch.player.name);

          /* COPY EQUIPMENT FILE */
          rc = old_rent_get_filename(new_ch.name, filename);
          printf("Opening input eqp: %s\r\n", filename);
          file_in = fopen(filename, "rb"); 
          if (!file_in) {
               bzero(buf, sizeof(buf));
               sprintf(buf, "WARNING: No EQUIPMENT rc=%d.\r\n\r\n",
                                  errno);
               printf(buf);
               fread(&new_ch, size_of_player_record, 1, file_player);
               continue;
          }

          /* READ HEADER */
          ptr = (char *)&old_head;
          bzero(ptr, size_of_old_head);
          fread(&old_head, size_of_old_head, 1, file_in);
          if (feof(file_in)) {
               printf("ERROR: Blank or invalid equipment header\r\n");
               fclose(file_in);
               fread(&new_ch, size_of_player_record, 1, file_player);
               continue;
          }

          /* READ FIRST EQUIPMENT RECORD FROM FILE */
          ptr = (char *)&old_elem;
          bzero(ptr, size_of_old_elem);
          fread(&old_elem, size_of_old_elem, 1, file_in);
          if (feof(file_in)) {
               printf("ERROR: Eqp file with only header - no eqp.\r\n");
               fclose(file_in);
               fread(&new_ch, size_of_player_record, 1, file_player);
               continue;
          }

          if (create_new_file == TRUE) {
              rc = new_rent_get_filename(new_ch.name, filename);
              printf("Opening output eqp: %s\r\n", filename);
              file_out = fopen(filename, "wb+"); 
              if (!file_out) {
                   bzero(buf, sizeof(buf));
                   sprintf(buf, "ERROR: %d opening equipment file.\r\n",
                                       errno);
                   printf(buf);
                   exit(1);
              }
              /* SET UP NEW HEADER */
              ptr = (char *)&new_head;
              bzero(ptr, size_of_new_head);
              strncpy(new_head.owner, old_head.owner, 19);
              new_head.total_cost  = convert_int(old_head.total_cost);
              new_head.last_update = convert_int(old_head.last_update);
              new_head.flags = convert_int(old_head.flags);

              /* WRITE NEW HEADER */
              fwrite(&new_head, size_of_new_head, 1, file_out); 
              number_written++;
              rc = errno;
              if (rc > 0 && rc != 2) {
                  printf("Error %d writing eqp head file.\r\n",rc);
                  abort();
              }
          }

          printf("Equipment:\r\n");

          for (lv_eqp_loop=0; (!feof(file_in)); lv_eqp_loop++) {

             /* MOVE STUFF FROM OLD STRUCT TO NEW STRUCT */
             ptr = (char *)&new_elem;
             bzero(ptr, size_of_new_elem);
             new_elem.item_number = convert_short(old_elem.item_number);
             printf("    equipment: %d\r\n", new_elem.item_number);
             new_elem.item_state  = convert_short(old_elem.item_state);
             new_elem.value[0]    = convert_int(old_elem.value[0]);
             new_elem.value[1]    = convert_int(old_elem.value[1]);
             new_elem.value[2]    = convert_int(old_elem.value[2]);
             new_elem.value[3]    = convert_int(old_elem.value[3]);
             new_elem.flags1      = convert_int(old_elem.flags1);
             new_elem.flags2      = convert_int(old_elem.flags2);
             new_elem.timer       = convert_int(old_elem.timer);
             new_elem.bitvector   = convert_int(old_elem.bitvector);
             for (idx=0; idx<MAX_OBJ_AFFECT; idx++) {
                  new_elem.affected[idx].location =
                             old_elem.affected[idx].location;
                  new_elem.affected[idx].modifier =
                             old_elem.affected[idx].modifier;
             }
             /* FINISHED DATA TRANSFER */

             if (create_new_file == TRUE) {
                  fwrite(&new_elem, size_of_new_elem, 1, file_out); 
                  number_written++;
                  rc = errno;
                  if (rc > 0 && rc != 2) {
                      printf("Error %d writing eqp elem file.\r\n",rc);
                      abort();
                  }
             }  

             ptr = (char *)&old_elem;
             bzero(ptr, size_of_old_elem);
             fread(&old_elem, size_of_old_elem, 1, file_in);

          } /* END OF for loop to read old equipment */  

          if (create_new_file == TRUE) {
              fclose(file_out);
          }
          fclose(file_in);
       } 
       fread(&new_ch, size_of_player_record, 1, file_player);

    } /* END OF for loop to read player file */  

    printf("There were %d player records.\r\n", lv_plr_loop);
    printf("Wrote %d records. Skipped %d.\r\n",
        number_written, lv_plr_loop - number_written);

    fclose(file_player);
    return (0);

} /* END OF main () */


int old_rent_get_filename(char *orig_name, char *filename) {

   char	*ptr, name[30];

   if (!*orig_name)
      return 0;

   strcpy(name, orig_name);
   for (ptr = name; *ptr; ptr++)
      *ptr = tolower(*ptr);

   bzero(filename, sizeof(filename));
   if (tolower(*name) >= 'a' && tolower(*name) <= 'z') {
       sprintf(filename, "%s/rentfiles/%c/%s.objs",
                   lv_dir_in, *name, name);
   }
   else {
      printf("ERROR: invalid rentfile for user: %s.\r\n", name);
      sprintf(filename, "%s/rentfiles/1/%s.objs",
                   lv_dir_in, name); 
   }

   return 1;

} /* END OF old_rent_get_filename() */

int new_rent_get_filename(char *orig_name, char *filename) {

   char	*ptr, name[30];

   if (!*orig_name)
      return 0;

   strcpy(name, orig_name);
   for (ptr = name; *ptr; ptr++)
      *ptr = tolower(*ptr);

   bzero(filename, sizeof(filename));
   if (tolower(*name) >= 'a' && tolower(*name) <= 'z') {
       sprintf(filename, "%s/rentfiles/%c/%s.objs",
                   lv_dir_out, *name, name);
   }
   else {
      printf("ERROR: invalid rentfile for user: %s.\r\n", name);
      sprintf(filename, "%s/rentfiles/1/%s.objs",
                   lv_dir_out, name); 
   }

   return 1;

} /* END OF new_rent_get_filename() */


sh_int convert_short(int numin) {

    union oldint_tag {
        sh_int  num;
        char sub[2];
    } oldint;

    union newint_tag {
        sh_int  num;
        char sub[2];
    } newint;

    if (gv_convert_flag == FALSE) {
        return (numin);
    }

    oldint.num = numin;
    newint.num = 0;
    newint.sub[0] = oldint.sub[1];
    newint.sub[1] = oldint.sub[0];
  /*
    printf("Old value: %d,  New value: %d\r\n",
                   oldint.num, newint.num);
  */
    return(newint.num);

} /* END OF convert_short() */


int convert_int(int numin) {

    union oldint_tag {
        int  num;
        char sub[4];
    } oldint;

    union newint_tag {
        int  num;
        char sub[4];
    } newint;

    if (gv_convert_flag == FALSE) {
        return (numin);
    }

    oldint.num = numin;
    newint.num = 0;
    newint.sub[0] = oldint.sub[3];
    newint.sub[1] = oldint.sub[2];
    newint.sub[2] = oldint.sub[1];
    newint.sub[3] = oldint.sub[0];
/*
    printf("Old value: %d,  New value: %d\r\n",
                   oldint.num, newint.num);
*/
    return(newint.num);

} /* END OF convert_int() */


int eqp500_alias_get_filename(
       char *orig_name, char *filename, int lv_direction) {

   char	*ptr,
        buf[MAX_STRING_LENGTH],
        name[30];

   if (!*orig_name)
      return 0;

   strcpy(name, orig_name);
   for (ptr = name; *ptr; ptr++)
      *ptr = tolower(*ptr);

   if (lv_direction == LOAD_ALIAS) {
       sprintf(buf, lv_dir_in);
   }
   else {
       sprintf(buf, lv_dir_out);
   }
   if (tolower(*name) >= 'a' && tolower(*name) <= 'z') {
       sprintf(filename, "%s/rentfiles/%c/%s.alias",
                   buf, *name, name);
   }
   else {
      printf("ERROR: invalid aliasfile for user: %s.\r\n", name);
      sprintf(filename, "%s/rentfiles/1/%s.alias", 
                   buf, name);
   }

   return 1;

} /* END OF eqp500_alias_get_filename() */


/* copied from db9800_load_save_alias() */
void eqp9800_load_save_aliases(
       struct char_data *ch, int lv_direction) {

     FILE *alias_file;
     int  rc,
          idx;
     char buf[MAX_STRING_LENGTH],
          buf2[MAX_STRING_LENGTH],
          lv_filename[MAX_INPUT_LENGTH];
  
     /* IF LOADING, ZERO OUT ALIAS AREA */
     if (lv_direction == LOAD_ALIAS) {
         bzero((char *)ch->player.alias_command[0],
                      MAX_ALIASES * (MAX_ALIAS_CMD + MAX_ALIAS_ACT));
     }

     /* GET A FILE NAME */
     bzero(lv_filename, sizeof(lv_filename));
     rc = eqp500_alias_get_filename(GET_NAME(ch), lv_filename, lv_direction);
     if (!rc) {
        bzero(buf, sizeof(buf));
        sprintf(buf, "Unable to get an alias filename for %s\r\n",
                             GET_NAME(ch));
        printf(buf);
        return;
     }

     /* IF SAVING, CONFIRM THERE ARE ALIASES */
     if (lv_direction == SAVE_ALIAS)  {
         /* PURGE OLD FILE NAME */
         unlink (lv_filename);
         for (idx = 0; idx < MAX_ALIASES; idx++) {
             if (*(ch->player.alias_command[idx])) {
                 idx = MAX_ALIASES + 1;  /* FORCE EXIT */
                 break;
             } 
         }
         if (idx < MAX_ALIASES + 1) { /* IT'LL BE EQUAL IF NONE FOUND */
             sprintf(buf,"    %s does not have any alias commands.\r\n",
                             GET_NAME(ch));
             printf(buf);
             return;
         }
     }
         
     /* OPEN FILE */
     alias_file = 0;
     if (lv_direction == LOAD_ALIAS) {
         printf("Opening input alias: %s\r\n", lv_filename);
         alias_file = fopen(lv_filename,"rb");
     }
     else {
         printf("Opening output alias: %s\r\n", lv_filename);
         alias_file = fopen(lv_filename,"wb");
     }

     if (!alias_file) {
        return;
     }

     for (idx = 0; idx < MAX_ALIASES; idx++) {
         if (lv_direction == LOAD_ALIAS)  {
             /* READ ALIASES FROM SAVE FILE */

             /* GET ALIAS  */
             bzero(buf, sizeof(buf));
             fgets(buf, 80, alias_file);
             if (errno < 1)
                  return;
             
             /* REMOVE CARRIAGE RETURN */
             if (strlen(buf)) 
                 buf[strlen(buf) - 1] = 0; 
             else 
                 buf[0] = 0;

             /* GET COMMAND */
             bzero(buf2, sizeof(buf2));
             fgets(buf2, 80, alias_file);
             if (errno < 1)
                  return;

             /* REMOVE CARRIAGE RETURN */
             if (strlen(buf2))
                buf2[strlen(buf2) - 1] = 0; 
             else 
                buf2[0] = 0;

             /* IF WE GOT BOTH ALIAS AND COMMAND, LOAD EM */
             if (*buf && *buf2) {
                 strncpy(ch->player.alias_command[idx], buf,
                                                 MAX_ALIAS_CMD - 1);
                 strncpy(ch->player.alias_actions[idx], buf2,
                                                 MAX_ALIAS_ACT - 1);
             }
         }

         if (lv_direction == SAVE_ALIAS)  {
             /* WRITE ALIASES FROM SAVE FILE */
             bzero(buf, sizeof(buf));
             if (*(ch->player.alias_command[idx])) {
                sprintf(buf, "%s\n%s\n",
                   ch->player.alias_command[idx],
                   ch->player.alias_actions[idx]);
                fputs(buf, alias_file);
                rc = errno; 
                if (!rc) {
                    bzero(buf, sizeof(buf));
                    sprintf(buf, "Error %d writing file %s for %s\r\n",
                             rc, lv_filename, GET_NAME(ch));
                    printf(buf);
                    return;
                }

             } /* END OF WE HAVE AN ALIAS */
         } /* END OF else SAVING */
     } /* END OF FOR LOOP */

     /* CLOSE FILE */
     fclose(alias_file);

} /* END OF eqp9800_load_save_aliases() */


