#ifndef WIDGET_H
#define WIDGET_H

struct widget
{
    SDL_Texture *default_tex, *select_tex, *interact_tex;
    void (*update)(struct widget *);
    void (*draw)(struct widget *);
    void (*select)(struct widget *);
    void (*interact)(struct widget *);

    SDL_Renderer *renderer;
    void *extra;
};
