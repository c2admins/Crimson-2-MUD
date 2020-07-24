
 /*********************************************************************
  *  file: rebuild.c , Player rebuild module.    Part of DIKUMUD       *
  *  Usage:  rebuild -u -v                                             *
  *********************************************************************/
  
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <ctype.h>
  #include <time.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <errno.h>
  
  #include "../structs.h"
  #include "../utility.h"
  #include "../db.h"
  #include "../comm.h"
  #include "../handler.h"
  #include "../limits.h"
  #include "../constants.h"
  #include "../ban.h"
  #include "../globals.h"
  #include "../weather.h"
 // #include "../old_char.h"

int gv_convert_flag;

int convert_int(int numin);
sh_int convert_short(int numin);

main (int argc, char **argv) {

    FILE *file_in,
         *file_out,
         *file_desc;

    time_t t;
    int verbose_print = FALSE,
        create_new_file = FALSE,
        add_hp,
        lv_level,
        lv_adjust_logoff_time, 
        lv_temp,
        size_of_old_record,
        size_of_new_record,
        loop,
        number_written,
        pos,
        rc,
        i, j;
   
    char *ptr,
         buf[MAX_STRING_LENGTH];
 
    struct char_file_u new_ch;

    /* IF USING THE SAME STRUCTURE, CHANGE NEXT TWO LINES */
   /* struct char_file_u_old old_ch;  
    size_of_old_record = sizeof(struct char_file_u_old); */
    struct char_file_u old_ch;
    size_of_old_record = sizeof(struct char_file_u);

    size_of_new_record = sizeof(struct char_file_u);
    printf("Use -c for integer conversion\r\n");
    printf("Use -v for verbose\r\n");
    printf("Use -u for update\r\n");
  
    /* A MEANS FOR ADJUSTING DOWNTIME ON THE MUD */
    lv_adjust_logoff_time = 0;

    gv_convert_flag = FALSE;
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

    printf("Opening input database: players.old\r\n");
    file_in = fopen("players.old", "rb+"); 
    if (!file_in) {
       bzero(buf, sizeof(buf));
       sprintf(buf, "ERROR: Unable to open input players.old file.\r\n");
       perror(buf);
       exit(1);
    }

    if (create_new_file == TRUE) {
        printf("Opening output database: players.new\r\n");
        file_out = fopen("players.new", "a"); 
        if (!file_out) {
           bzero(buf, sizeof(buf));
           sprintf(buf, "ERROR: Unable to open output players.old file.\r\n");
           perror(buf);
           exit(1);
        }
    }

    t = time(NULL);
    printf("Running rebuild at: %s\r\n", ctime(&t));
  
    fread(&old_ch, size_of_old_record, 1, file_in);
    for (loop=0; (!feof(file_in)); loop++) {
       if (*old_ch.name ==  0  || /* THROW OUT NULLS   */
           *old_ch.name == 32  || /* THROW OUT BLANKS  */
           *old_ch.name == '1' || /* THROW OUT DELETES */
           *old_ch.name == '2' || /* THROW OUT SELF DELETES */
           loop > 100000) {
               if (verbose_print == TRUE) { 
                   sprintf(buf, "Skipping %d name= %s\r\n",
                                          loop, old_ch.name);
                   printf(buf);
               }
       }
       else {
          /* MOVE STUFF FROM OLD STRUCT TO NEW STRUCT */
          ptr = (char *)&new_ch;
          bzero(ptr, sizeof(new_ch));
         

          new_ch.sex = old_ch.sex;
          new_ch.class = old_ch.class;
          new_ch.race = old_ch.race;
          new_ch.level = old_ch.level;
          new_ch.birth = convert_int(old_ch.birth);

          new_ch.played = convert_int(old_ch.played);
          new_ch.last_logon = convert_int(old_ch.last_logon);
          new_ch.logoff_time = convert_int(old_ch.logoff_time);
          new_ch.logoff_time += lv_adjust_logoff_time;
          if (new_ch.logoff_time > time(0) ||
              new_ch.logoff_time < 0) {
                new_ch.logoff_time = time(0);
              printf("ERROR: %s's logoff time is invalid - defaulting to current time.\r\n",
                     old_ch.name);
          }

          new_ch.how_player_left_game = old_ch.how_player_left_game;

          new_ch.weight = old_ch.weight;
          new_ch.height = old_ch.height;
          strcpy(new_ch.title, old_ch.title);
          strcpy(new_ch.description, old_ch.description);
          strcpy(new_ch.immortal_enter, old_ch.immortal_enter);
          strcpy(new_ch.immortal_exit, old_ch.immortal_exit);

          new_ch.start_room       = convert_short(old_ch.start_room);
          new_ch.private_room     = convert_short(old_ch.private_room);
          new_ch.visible_level    = old_ch.visible_level;

          new_ch.abilities.str    = old_ch.abilities.str;
          new_ch.abilities.intel  = old_ch.abilities.intel;
          new_ch.abilities.wis    = old_ch.abilities.wis;
          new_ch.abilities.dex    = old_ch.abilities.dex;
          new_ch.abilities.con    = old_ch.abilities.con;
          new_ch.abilities.cha    = old_ch.abilities.cha;

          new_ch.points.mana      = convert_int(old_ch.points.mana);
          new_ch.points.max_mana  = convert_int(old_ch.points.max_mana);
          new_ch.points.hit       = convert_int(old_ch.points.hit);
          new_ch.points.max_hit   = convert_int(old_ch.points.max_hit);
          new_ch.points.move      = convert_int(old_ch.points.move);
          new_ch.points.max_move  = convert_int(old_ch.points.max_move);
          new_ch.points.armor     = convert_short(old_ch.points.armor);

          new_ch.points.gold = convert_int(old_ch.points.gold);
          if (new_ch.points.gold < 0) {
              printf("ERROR: Setting %s gold from %d to ZERO.\r\n",
                     old_ch.name, old_ch.points.gold);
              new_ch.points.gold = 0;
          }

          new_ch.points.inn_gold = convert_int(old_ch.points.inn_gold);
          if (new_ch.points.inn_gold < 0)  {
              printf("ERROR: Setting %s inn gold from %d to ZERO.\r\n",
                     old_ch.name, old_ch.points.inn_gold);
              new_ch.points.inn_gold  = 0;
          }

          new_ch.points.bank_gold =convert_int(old_ch.points.bank_gold);
          if (new_ch.points.bank_gold < 0)  {
              printf("ERROR: Setting %s bank gold from %d to ZERO.\r\n",
                     old_ch.name, old_ch.points.bank_gold);
              new_ch.points.bank_gold  = 0;
          }

          new_ch.points.exp      = convert_int(old_ch.points.exp);
          new_ch.points.hitroll  = old_ch.points.hitroll;
          new_ch.points.damroll  = old_ch.points.damroll;
          new_ch.points.fate     = 0;
          new_ch.points.stat_gold= convert_int(old_ch.points.stat_gold);

          for (j=0; j < 150; j++) {
              new_ch.skills[j].learned = old_ch.skills[j].learned;
          } for (j=150; j < 250; j++) {
             new_ch.skills[j].learned = 0; }

          for (j=0; j < MAX_AFFECT && old_ch.affected[j].type; j++) {
                new_ch.affected[j].type = old_ch.affected[j].type;
                new_ch.affected[j].duration = 
                            convert_short(old_ch.affected[j].duration);
                new_ch.affected[j].modifier = old_ch.affected[j].modifier;
                new_ch.affected[j].location = old_ch.affected[j].location;
                new_ch.affected[j].bitvector = 
                             convert_int(old_ch.affected[j].bitvector);
                new_ch.affected[j].next = old_ch.affected[j].next;
          } /* END OF j LOOP */

          for (j=0; j < 5; j++) {
            new_ch.apply_saving_throw[j] = 
                        convert_short(old_ch.apply_saving_throw[j]);
          }

          for (j=0; j < MAX_CONDITIONS; j++) {
              new_ch.conditions[j] =convert_short(old_ch.conditions[j]);
          }

          new_ch.spells_to_learn = old_ch.spells_to_learn;
          new_ch.alignment = convert_int(old_ch.alignment);

          new_ch.act  = convert_int(old_ch.act);
          new_ch.act2 = convert_int(old_ch.act2);
          new_ch.act3 = convert_int(old_ch.act3);
          new_ch.act4 = convert_int(old_ch.act4);

          strcpy(new_ch.name, old_ch.name);
          /* CONVERT FIRST LETTER TO UPPERCASE */
          if (new_ch.name[0] >= 'a' && new_ch.name[0] <= 'z')
                       new_ch.name[0] - 32;
          strcpy(new_ch.pwd, old_ch.pwd);

          new_ch.time_of_muzzle = old_ch.time_of_muzzle;
          new_ch.descriptor_flag1 = old_ch.descriptor_flag1;
          strncpy(new_ch.email_name, old_ch.email_name, EMAIL_SIZE);

          /* FINISHED PLAYER TRANSFER */

          if (verbose_print == TRUE) { 
            printf("\r\n    *************************************\r\n");
            printf("%d %d %15s %s\r\n", 
                        loop, new_ch.level, new_ch.name, new_ch.title);
            printf("    %s\r\n", new_ch.immortal_enter);
            printf("    %s\r\n", new_ch.immortal_exit);
            printf("%s\r\n", new_ch.description);
            printf("    sex: %d  race: %d class: %d  weight: %d  height: %d\r\n",
                        new_ch.sex, new_ch.race, new_ch.class, new_ch.weight, new_ch.height);
            printf("    room: %d  spells2learn: %d  alignment: %d\r\n",
                        new_ch.start_room, new_ch.spells_to_learn,
                                                new_ch.alignment);
            printf("    act flag: %d\r\n",
                        new_ch.act);
            printf("    str %d  Int %d  Wis %d  Dex %d  Con %d Cha %d\r\n",
                        new_ch.abilities.str,
                        new_ch.abilities.intel,
                        new_ch.abilities.wis,
                        new_ch.abilities.dex,
                        new_ch.abilities.con,
                        new_ch.abilities.cha);
            printf("    saving throws  %d   %d   %d   %d   %d\r\n",
                        new_ch.apply_saving_throw[0],
                        new_ch.apply_saving_throw[1],
                        new_ch.apply_saving_throw[2],
                        new_ch.apply_saving_throw[3],
                        new_ch.apply_saving_throw[4]);
            printf("    mana: %d/%d  hit: %d/%d  move: %d/%d\r\n",
                        new_ch.points.mana,
                        new_ch.points.max_mana,
                        new_ch.points.hit,
                        new_ch.points.max_hit,
                        new_ch.points.move,
                        new_ch.points.max_move);
            printf("    Armor: %d  Gold: %d  Bank: %d  Inn: %d Stat: %d\r\n",   
                        new_ch.points.armor,
                        new_ch.points.gold,
                        new_ch.points.bank_gold,
                        new_ch.points.inn_gold,
                        new_ch.points.stat_gold);
            printf("    exp: %d  Hitroll: %d  Damroll: %d\r\n",
                        new_ch.points.exp,
                        new_ch.points.hitroll,
                        new_ch.points.damroll);
            printf("    last_login: %s",
                        ctime(&new_ch.last_logon));
            printf("    SPELLS\r\n");
            for (j=0; j<25 && new_ch.affected[j].type; j++) {
                printf("        %d type: %d left: %d  mod: %d loc: %d bit: %d next:%d\r\n",
                        j,
                        new_ch.affected[j].type,
                        new_ch.affected[j].duration,
                        new_ch.affected[j].modifier,
                        new_ch.affected[j].location,
                        new_ch.affected[j].bitvector,
                        new_ch.affected[j].next);
            } /* END OF j LOOP */
          } /* END OF DO PRINT */             
          if (create_new_file == TRUE) {
             fwrite(&new_ch, sizeof(struct char_file_u), 1, file_out); 
             number_written++;
             rc = errno;
             if (rc > 0) {
                printf("Error %d writing output player file\r\n", rc);
             }
          }
       } 
       fread(&old_ch, size_of_old_record, 1, file_in);

    } /* END OF for */  
    printf("There were %d player records in original file.\r\n", loop);
    printf("Wrote %d records. Skipped %d.\r\n",
        number_written, loop - number_written);

    return (0);

} /* END OF main () */


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

