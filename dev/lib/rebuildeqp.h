
struct old_obj_file_elem {
     sh_int item_number;
     sh_int item_state;
     int value[4];
     int flags1;
     int flags2;
     int timer;
     long bitvector;
     struct obj_affected_type affected[MAX_OBJ_AFFECT];
};

