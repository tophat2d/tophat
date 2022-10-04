THEXT(th_vf2, th_atlas_nth_coords, th_atlas *, uu);
THEXT(th_rect, th_atlas_get_cell, th_atlas *, th_vf2);

THEXT(void, th_audio_init);
THEXT(void, th_audio_deinit);

THEXT(int, th_canvas_compile_shader, char *, char *);
THEXT(void, th_canvas_rect, uint32_t, th_rect);
THEXT(void, th_canvas_line, uint32_t, th_vf2, th_vf2, fu);
THEXT(void, th_canvas_text, char *, uint32_t, th_vf2, fu);
THEXT(void, th_canvas_triangle, uint32_t, th_vf2, th_vf2, th_vf2);
THEXT(void, th_canvas_flush);

THEXT(int, th_line_to_line, th_vf2, th_vf2, th_vf2, th_vf2, th_vf2 *);
THEXT(uu, th_point_to_quad, th_vf2, th_quad *, th_vf2 *);
THEXT(uu, th_quad_to_quad, th_quad *, th_quad *, th_vf2 *);
THEXT(uu, th_ent_to_ent, th_ent *, th_ent *, th_vf2 *);
THEXT(uu, th_line_to_quad, th_vf2, th_vf2, th_quad *q, th_vf2 *);

THEXT(void, th_ent_draw, th_ent *, th_rect *);
THEXT(void, th_ent_getcoll, th_ent *, th_ent **, uu, uu, uu, th_coll *);

THEXT(uint32_t, th_color_rgb, float, float, float, float);
THEXT(uint32_t, th_color_hsv2rgb, float, float, float, float);

THEXT(th_font *, th_font_load, char *, double, uint32_t);
THEXT(void, th_font_draw, th_font *, const char *, double, double, uint32_t, double);
THEXT(th_vf2, th_font_measure, th_font *, const char *);

THEXT(th_image *, th_load_image, char *);
THEXT(void, th_image_free, th_image *);
THEXT(void, th_image_from_data, th_image *, uint32_t *, th_vf2);
THEXT(void, th_blit_tex, th_image *, th_quad, uint32_t);
THEXT(void, th_image_render_transformed, th_image *, th_transform trans, uint32_t);
THEXT(void, th_image_set_filter, th_image *, int);
THEXT(void, th_image_update_data, th_image *, uint32_t *, th_vf2);
THEXT(void, th_image_compile_shader, char *, char *);
THEXT(void, th_image_set_as_render_target, th_image *);
THEXT(void, th_image_remove_render_target, th_rect *);

THEXT(void, th_error);
THEXT(void, th_calculate_scaling, float, float);

THEXT(void, th_particles_draw, th_particles *, th_rect, int);

THEXT(th_vf2, th_quad_min, th_quad );
THEXT(void, th_transform_rect, th_quad *, th_transform, th_rect);
THEXT(void, th_transform_quad, th_quad *, th_transform);
THEXT(void, th_transform_vf2, th_vf2 *, th_transform);

THEXT(void, th_rotate_point, th_vf2 *, th_vf2 *, fu);
THEXT(void, th_vector_normalize, float *, float *);

THEXT(void, th_ray_getcoll, th_ray *, th_coll *, int, int *, th_ent **, int);

THEXT(void, th_tmap_draw, th_tmap *, th_rect *);
THEXT(void, th_tmap_autotile, uu *, uu *, uu, uu, uu *, uu);

THEXT(void, th_utf8_decode, uint32_t *, const char *);
THEXT(void, th_utf8_encode, char *, uint32_t);
