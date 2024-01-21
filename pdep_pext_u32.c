
//From Hacker's Delight chapter 7.
//Taken from this post https://stackoverflow.com/questions/77834169/what-is-a-fast-fallback-algorithm-which-emulates-pdep-and-pext-in-software/77839918#77839918
//
// Equivalent to _pdep_u32()
unsigned expand(unsigned x, unsigned m) {
    unsigned m0, mk, mp, mv, t;
    unsigned array[5];
    int i;
    m0 = m; // Save original mask.
    mk = ~m << 1; // We will count 0's to right.
    for (i = 0; i < 5; i++) {
        mp = mk ^ (mk << 1); // Parallel suffix.
        mp = mp ^ (mp << 2);
        mp = mp ^ (mp << 4);
        mp = mp ^ (mp << 8);
        mp = mp ^ (mp << 16);
        mv = mp & m; // Bits to move.
        array[i] = mv;
        m = (m ^ mv) | (mv >> (1 << i)); // Compress m.
        mk = mk & ~mp;
    }
    for (i = 4; i >= 0; i--) {
        mv = array[i];
        t = x << (1 << i);
        x = (x & ~mv) | (t & mv);
    }
    return x & m0; // Clear out extraneous bits.
}
// Equivalent to _pext_u32()
unsigned compress(unsigned x, unsigned m) {
    unsigned mk, mp, mv, t;
    int i;
    x = x & m; // Clear irrelevant bits.
    mk = ~m << 1; // We will count 0's to right.
    for (i = 0; i < 5; i++) {
        mp = mk ^ (mk << 1); // Parallel suffix.
        mp = mp ^ (mp << 2);
        mp = mp ^ (mp << 4);
        mp = mp ^ (mp << 8);
        mp = mp ^ (mp << 16);
        mv = mp & m; // Bits to move.
        m = (m ^ mv) | (mv >> (1 << i)); // Compress m.
        t = x & mv;
        x = (x ^ t) | (t >> (1 << i)); // Compress x.
        mk = mk & ~mp;
    }
    return x;
}
//=======================================================================================================================
//Adapted from the above by Simon Goater Jan 2024
//=======================================================================================================================
/* For usage like the following...

    pdepu32_mask_obj_t mask_obj;
    pdep_u32_init(mask, &mask_obj);
    for (some loop) { ...
      res = pdep_u32_calc(src, mask_obj);
    ...}
*/

typedef struct {
    uint32_t array[5];
    uint32_t mask;
} pdepu32_mask_obj_t;

typedef struct {
    uint32_t mv[5];
    uint32_t mask;
} pextu32_mask_obj_t;

void pext_u32_init(uint32_t m, pextu32_mask_obj_t *mask_obj) {
    uint32_t mk, mp, mv;
    uint32_t i;
    mask_obj->mask = m;
    mk = ~m << 1; // We will count 0's to right.
    for (i = 0; i < 5; i++) {
        mp = mk ^ (mk << 1); // Parallel suffix.
        mp = mp ^ (mp << 2);
        mp = mp ^ (mp << 4);
        mp = mp ^ (mp << 8);
        mp = mp ^ (mp << 16);
        mv = mp & m; // Bits to move.
        mask_obj->mv[i] = mv; // Bits to move.
        m = (m ^ mv) | (mv >> (1 << i)); // Compress m.
        mk = mk & ~mp;
    }
    return;
}

uint32_t pext_u32_calc(uint32_t x, pextu32_mask_obj_t mask_obj) {
    uint32_t i, t;
    x = x & mask_obj.mask; // Clear irrelevant bits.
    for (i = 0; i < 5; i++) {
        t = x & mask_obj.mv[i];
        x = (x ^ t) | (t >> (1 << i)); // Compress x.
    }
    return x;
}

void pdep_u32_init(uint32_t m, pdepu32_mask_obj_t *mask_obj) {
    uint32_t mk, mp, mv;
    int32_t i;
    mask_obj->mask = m;
    mk = ~m << 1; // We will count 0's to right.
    for (i = 0; i < 5; i++) {
        mp = mk ^ (mk << 1); // Parallel suffix.
        mp = mp ^ (mp << 2);
        mp = mp ^ (mp << 4);
        mp = mp ^ (mp << 8);
        mp = mp ^ (mp << 16);
        mv = mp & m; // Bits to move.
        mask_obj->array[i] = mv;
        m = (m ^ mv) | (mv >> (1 << i)); // Compress m.
        mk = mk & ~mp;
    }
    return; 
}

uint32_t pdep_u32_calc(uint32_t x, pdepu32_mask_obj_t mask_obj) {
    uint32_t mv, t;
    uint32_t m = mask_obj.mask;
    int32_t i;
    for (i = 4; i >= 0; i--) {
        mv = mask_obj.array[i];
        t = x << (1 << i);
        x = (x & ~mv) | (t & mv);
    }
    return x & m; // Clear out extraneous bits.
}
//=======================================================================================================================

