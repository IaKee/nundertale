//Headers utilizados
#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include "allegro5/allegro_image.h"
#include <allegro5/allegro_ttf.h>
#include "allegro5/allegro_color.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "allegro5/allegro_primitives.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>

//Atributos da janela
#define LARGURA_TELA 640
#define ALTURA_TELA 480

typedef struct location
{ //Struct com posição x e y
    int posicao_x;
    int posicao_y;
}POS;
typedef struct obj
{
    int wx, wy;
    float posx, posy;
    int w, h;
    int movespeed;
    int velx, vely;
    int dir;
    int scale_x;
    int scale_y;
    int animation_delay;
}MOB_PROP;

//Janela principal
ALLEGRO_DISPLAY *janela = NULL;

//Ponteiros para arquivos utilizados (podem variar, são fechados e abertos conforme necessidade)
ALLEGRO_BITMAP *imagem1 = NULL;
ALLEGRO_BITMAP *imagem2 = NULL;
ALLEGRO_FONT *font1 = NULL;
ALLEGRO_FONT *font2 = NULL;
ALLEGRO_FONT *font3 = NULL;
ALLEGRO_FONT *font4 = NULL;
ALLEGRO_FONT *font5 = NULL;

int n_faixas = 5; //Numero de samples (atualizar)
ALLEGRO_SAMPLE *music1 = NULL;
ALLEGRO_SAMPLE *sound1 = NULL;
ALLEGRO_SAMPLE *sound2 = NULL;
ALLEGRO_SAMPLE *sound3 = NULL;
ALLEGRO_SAMPLE *sound4 = NULL;
ALLEGRO_SAMPLE *sound5 = NULL;
ALLEGRO_SAMPLE *sound6 = NULL;
ALLEGRO_SAMPLE *sound7 = NULL;
ALLEGRO_SAMPLE_ID *music1_id = NULL;
ALLEGRO_SAMPLE_ID *sound1_id = NULL;
ALLEGRO_SAMPLE_ID *sound2_id = NULL;
ALLEGRO_SAMPLE_ID *sound3_id = NULL;
ALLEGRO_SAMPLE_ID *sound4_id = NULL;
ALLEGRO_SAMPLE_ID *sound5_id = NULL;
ALLEGRO_SAMPLE_ID *sound6_id = NULL;
ALLEGRO_SAMPLE_ID *sound7_id = NULL;
ALLEGRO_SAMPLE_INSTANCE *music1_instance = NULL;
ALLEGRO_SAMPLE_INSTANCE *sound1_instance = NULL;
ALLEGRO_SAMPLE_INSTANCE *sound2_instance = NULL;
ALLEGRO_SAMPLE_INSTANCE *sound3_instance = NULL;
ALLEGRO_SAMPLE_INSTANCE *sound4_instance = NULL;
ALLEGRO_SAMPLE_INSTANCE *sound5_instance = NULL;
ALLEGRO_SAMPLE_INSTANCE *sound6_instance = NULL;
ALLEGRO_SAMPLE_INSTANCE *sound7_instance = NULL;

ALLEGRO_TRANSFORM camera;

ALLEGRO_EVENT_QUEUE *queue = NULL;
ALLEGRO_EVENT event;
ALLEGRO_KEYBOARD_STATE* kbd;

//float cameraPosition[2]={0,0};

float scale = 1.0;
float degrees = 0;
bool closed = false;
char heroname[15] = "";
double tempo_inicio, tempo_atual;
int tempo_segundos;
int eflag;

//Controle de easter eggs
bool gamer = false;
bool pro = false;
bool pro_gamer = false;
bool ultra_gamer = false;
bool rmt = false;

//função que mostra mensagem de erro em uma caixa de texto nativa
//a função recebe a string da mensagem como parâmetro
void passou(int n)
{
    printf("\npassou por aqui - checkpoint %d\n", n);
}
void CameraUpdate(float *cameraPosition, float x, float y, int width, int height)
{
    cameraPosition[0] = -(LARGURA_TELA/2) + (x + width/2);
    cameraPosition[1] = -(ALTURA_TELA/2) + (x + height/2);

    if(cameraPosition[0] < 0)
        cameraPosition[0] = 0;
    if(cameraPosition[1] < 0)
        cameraPosition[1] = 0;
}
bool togglebool (bool boolean)
{
    if(boolean == true)
        return false;
    else
        return true;
}
bool allCharactersSame(char* s)
{
    int n = strlen(s);
    for (int i = 1; i < n; i++)
        if (s[i] != s[0])
            return false;
    return true;
}
void error_msg(char *text)
{ //Exibe uma mensagem de erro com a string enviada por referencia
    al_show_native_message_box(NULL,"N-undertale foi pras cucuias!", "Erro fatal =/", text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}
void lore_fade_in_img(char* img_path, int velocidade, int pos_X, int pos_y)
{ //Lore inicial lá, só que com um fade na imagem que aparece, respeitando o loop de close
    imagem1 = al_load_bitmap(img_path);

    if (velocidade < 0)
    {
        velocidade = 1;
    }
    else
        if (velocidade > 15)
        {
            velocidade = 15;
        }

    for (int alfa = 0; alfa <= 255; alfa += velocidade)
    {
        //Permite fechar o jogo durante o fade
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return;
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);

        al_clear_to_color(al_map_rgb(0, 0, 0)); //Imprime um quadrado preto no fundo

        al_draw_tinted_bitmap(imagem1, al_map_rgba(alfa, alfa, alfa, alfa), pos_X, pos_y, 0);
        al_flip_display();
        al_rest(0.005); // Não é necessário caso haja controle de FPS
    }
}
void fade_to_black(int velocidade)
{ //Da um fade bonitinho para preto com a velocidade definida (de 1 a 15) - pode salvar o jogo

    ALLEGRO_BITMAP *buffer = NULL;
    buffer = al_create_bitmap(LARGURA_TELA, ALTURA_TELA);
    al_set_target_bitmap(buffer);
    al_draw_bitmap(al_get_backbuffer(janela), 0, 0, 0);
    al_set_target_bitmap(al_get_backbuffer(janela));

    if (velocidade <= 0)
    {
        velocidade = 1;
    }
    else if (velocidade > 15)
    {
        velocidade = 15;
    }

    int alfa;
    for (alfa = 0; alfa <= 255; alfa += velocidade)
    {
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_tinted_bitmap(buffer, al_map_rgba(255 - alfa, 255 - alfa, 255 - alfa, alfa), 0, 0, 0);
        al_flip_display();
        al_rest(0.005); // Não é necessário caso haja controle de FPS
    }

      al_destroy_bitmap(buffer);
      return;
}
void fade_to_white(int velocidade)
{ //Da um fade bonitinho para preto com a velocidade definida (de 1 a 15) - pode salvar o jogo

    ALLEGRO_BITMAP *buffer = NULL;
    buffer = al_get_target_bitmap();
    al_set_target_bitmap(buffer);
    //al_draw_bitmap(al_get_backbuffer(janela), 0, 0, 0);
    al_set_target_bitmap(buffer);

    if (velocidade <= 0)
    {
        velocidade = 1;
    }
    else if (velocidade > 15)
    {
        velocidade = 15;
    }

    int alfa;
    int idx = 255;
    for (alfa = 255; alfa >= 0; alfa -= velocidade)
    {
        al_clear_to_color(al_map_rgb(255,255,255));
        al_draw_tinted_bitmap(buffer, al_map_rgb(255 - alfa, 255 - alfa, 255 - alfa), 0, 0, 0);
        al_flip_display();
        al_rest(0.015); // Não é necessário caso haja controle de FPS
    }

      return;
}
int inicializa()
{ //Inicializa os principais recursos da biblioteca allegro

    //FUNC_EXEC(-1, al_init);
    if(!al_init())
        return -1;

    al_init_font_addon();
    if(!al_init_ttf_addon())
        return -2;

    queue = al_create_event_queue();
    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);

    if(!al_install_audio())
        return -3;
    if(!al_init_acodec_addon())
        return -4;

    al_reserve_samples(n_faixas);

    if(!al_install_keyboard())
        return -5;

    if(!al_init_image_addon())
        return -6;
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(janela));

    imagem1 = al_load_bitmap("icon_png.png");
    al_set_window_title(janela, "NOT UNDERTALE"); //Muda o titulo da janela para NOT UNDERTALE
    al_set_display_icon(janela, imagem1);

    return 0;
}
int disclaimer_nundertale()
{ //Imprime o logo do nundertale, fica na tela por 20 segundos (pressione qualquer tecla para skip) - com disclaimer

    int timing = 0;
    float fade = 0;

    sound1 = al_load_sample( "discreepy.ogg" );
    al_play_sample(sound1, 0.4, 0.0,1.0,ALLEGRO_PLAYMODE_ONCE, NULL);


    while (timing < 200)
    {
        if(fade < 255)
        {
            al_clear_to_color(al_map_rgb(0, 0, 0)); //Colore a tela de preto
            font1 = al_load_font("undet_font.otf", 48, 0);
            al_draw_text(font1, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, ALTURA_TELA/2-100, ALLEGRO_ALIGN_CENTRE, "Not Undertale");
            font2 = al_load_font("gomarice_shmup_in_the_zone.ttf", 35, 0);
            al_draw_text(font2, al_map_rgb(fade, 0, 0), LARGURA_TELA/2, 40, ALLEGRO_ALIGN_CENTRE, "Trigger Warning");
            font2 = al_load_font("gomarice_shmup_in_the_zone.ttf", 28, 0);
            al_draw_text(font2, al_map_rgb(fade, 0, 0), LARGURA_TELA/2, 100, ALLEGRO_ALIGN_CENTRE, "This is");
            font2 = al_load_font("gomarice_shmup_in_the_zone.ttf", 14, 0);

            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*0, ALLEGRO_ALIGN_CENTRE, " - This game may and probably will be offensive to some people -");

            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*2, ALLEGRO_ALIGN_CENTRE, "It was made this way to be a comical parody of the UNDERTALE title");
            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*3, ALLEGRO_ALIGN_CENTRE, "however it is not supposed to be associated with the original game");
            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*4, ALLEGRO_ALIGN_CENTRE, "or creator, as we don't want to deprecate their public image.");

            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*6, ALLEGRO_ALIGN_CENTRE, "This piece of content was developed solely for an entertainment");
            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*7, ALLEGRO_ALIGN_CENTRE, "video for our youtube channel at www.youtube.com/ruimmastime");
            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*8, ALLEGRO_ALIGN_CENTRE, "This game was not officially distributed to the public in other");
            al_draw_text(font2, al_map_rgb(fade, fade, fade), LARGURA_TELA/2, 220+20*9, ALLEGRO_ALIGN_CENTRE, "form than on video footage.");
            al_flip_display();

            fade += 45;
        }

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }

       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            al_stop_samples();
            return 0;
        }

        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);

        al_rest(0.1);
        timing++;
    }

    fade_to_black(1);

    return 0;
}
int fade_with_text(char* string, int text_posx, int text_posy, int text_endx, int fade_velocity)
{ //Fade com um texto, pressione qualquer tecla para continuar...
    fade_to_black(fade_velocity);

    font1 = al_load_font("nundet_m.ttf", 36, 0);
    al_draw_justified_text(font1, al_map_rgb_f(1, 1, 1), text_posx, text_endx, text_posy, 54564, ALLEGRO_ALIGN_INTEGER, string);
    al_flip_display();

    while(!closed)
    {
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }

       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            al_stop_samples();
            return 0;
        }

        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    return -1;
}
int init_lore()
{ //Aquela lore nada a ver que passa td a vez que tu abre o jogo

    //Carrega toda a midia utilizada nesse método
    music1 = al_load_sample("Undertale - Once Upon A Time Remix (ft Yuri, zR).ogg");
    music1_instance = al_create_sample_instance(music1);
    al_set_sample_instance_playmode(music1_instance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(music1_instance, al_get_default_mixer());
    al_play_sample_instance(music1_instance);

    sound3 = al_load_sample("assets/sound effects/menu_type.ogg");
    sound3_instance = al_create_sample_instance(sound3);
    al_set_sample_instance_playmode(sound3_instance, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(sound3_instance, al_get_default_mixer());
    al_set_sample_instance_gain(sound3_instance, 0.5); //Diminui o volume pela metade

    sound5 = al_load_sample("assets/sound effects/pintomole.ogg");
    sound5_instance = al_create_sample_instance(sound5);
    al_set_sample_instance_playmode(sound5_instance, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(sound5_instance, al_get_default_mixer());
    al_set_sample_instance_gain(sound5_instance, 1); //Diminui o volume pela metade

    sound6 = al_load_sample("assets/sound effects/broken_legs.ogg");
    sound6_instance = al_create_sample_instance(sound6);
    al_set_sample_instance_playmode(sound6_instance, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(sound6_instance, al_get_default_mixer());
    al_set_sample_instance_gain(sound6_instance, 0.5); //Diminui o volume pela metade

    font1 = al_load_font("nundet_m.ttf", 36, 0);

    bool skip_request = false;
    bool continua = false;

    char string1[] = "Not so long ago, two racists";
    char string2[] = "ruled over the internet:";
    char string3[] = "FURRIES and GAMERS.";
    char stringx[50] = "";
    int lore_img_posx = 121;
    int lore_img_posy = 62;
    int textx = 125;
    int text1y = 3*ALTURA_TELA/4-50;
    int text2y = 3*ALTURA_TELA/4-10;
    int text3y = 3*ALTURA_TELA/4+30;
    int fadespeed = 5;
    int timing = 0;


    //************************************************************************************************LORE CARD 1
    lore_fade_in_img("assets/img/lore1.bmp", fadespeed, lore_img_posx, lore_img_posy);
    for(int f=0; f < sizeof(string1); f++)
    { //Linha 1, card 1
        stringx[f]=string1[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    if(!skip_request)
        al_rest(.5);
    for(int f=0; f < sizeof(string2); f++)
    { //Linha 2, card 1
        stringx[f]=string2[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string1); //A linha 1 ja esta completa
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    if(!skip_request)
        al_rest(1);
    for(int f=0; f < sizeof(string3); f++)
    { //Linha 3, card 1
        stringx[f]=string3[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string1); //A linha 1 ja esta completa
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, string2); //A linha 2 ja esta completa
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text3y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string3[f]==' ')
                random_rest+=1;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    continua = false;
    timing = 0;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }

       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }

        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 2
    lore_fade_in_img("assets/img/lore2.bmp", fadespeed, lore_img_posx, lore_img_posy);
    char string4[] = "One day, the FURRIES tried";
    char string5[] = "yiffing the GAMERS";
    char string6[] = "like true degenerates.";
    skip_request = false;
    for(int f=0; f < sizeof(string4); f++)
    { //Linha 1, card 2
        stringx[f]=string4[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string5); f++)
    { //Linha 2, card 2
        stringx[f]=string5[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string4);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string6); f++)
    { //Linha 3, card 2
        stringx[f]=string6[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string4);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, string5);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text3y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 3
    lore_fade_in_img("assets/img/lore3.bmp", fadespeed, lore_img_posx, lore_img_posy);
    char string7[] = "And after some long reddit";
    char string8[] = "posts the GAMERS defeated";
    char string9[] = "those degenerates.";
    skip_request = false;
    for(int f=0; f < sizeof(string7); f++)
    { //Linha 1, card 3
        stringx[f]=string7[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string8); f++)
    { //Linha 2, card 3
        stringx[f]=string8[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string7);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string9); f++)
    { //Linha 3, card 3
        stringx[f]=string9[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string7);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, string8);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text3y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 4
    lore_fade_in_img("assets/img/lore4.bmp", fadespeed, lore_img_posx, lore_img_posy);
    char string10[] = "They locked the degenerates";
    char string11[] = "underground using pro gamer";
    char string12[] = "moves and moutain dew(ritos).";
    skip_request = false;
    for(int f=0; f < sizeof(string10); f++)
    { //Linha 1, card 4
        stringx[f]=string10[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string11); f++)
    { //Linha 2, card 4
        stringx[f]=string11[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string10);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string12); f++)
    { //Linha 3, card 4
        stringx[f]=string12[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string10);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, string11);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text3y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]==',')
                random_rest+=0.5;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    char string13[] = "Some weeks later.....";
    skip_request = false;
    for(int f=0; f < sizeof(string13); f++)
    { //Linha 1, card 3
        stringx[f]=string13[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string1[f]=='.')
                random_rest+=1;
            al_rest(random_rest);
            al_play_sample_instance(sound3_instance);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 5
    lore_fade_in_img("assets/img/lore5.bmp", fadespeed, lore_img_posx, lore_img_posy);
    char string14[] = "<<................MTn. DEW................>>";
    char string15[] = "<<...................202X....................>>";
    skip_request = false;
    for(int f=0; f < sizeof(string14); f++)
    { //Linha 1, card 5
        stringx[f]=string14[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string14[f]=='.')
                random_rest/=100;
            al_play_sample_instance(sound3_instance);
            al_rest(random_rest);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string15); f++)
    { //Linha 1, card 4
        stringx[f]=string15[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string14);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string14[f]=='.')
                random_rest/=100;
            al_play_sample_instance(sound3_instance);
            al_rest(random_rest);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    //lore_skip("lore5.bmp", "MTn. DEW", "    202X", "", 275, 280, 450);
    fade_to_black(10);
    //************************************************************************************************LORE CARD 6
    lore_fade_in_img("assets/img/lore6.bmp", fadespeed, lore_img_posx, lore_img_posy);
    char string16[] = "Lengend says that those who";
    char string17[] = "keep on yiffin may never";
    char string18[] = "game again...";
    skip_request = false;
    for(int f=0; f < sizeof(string16); f++)
    { //Linha 1, card 6
        stringx[f]=string16[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            al_play_sample_instance(sound3_instance);
            al_rest(random_rest);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string17); f++)
    { //Linha 2, card 6
        stringx[f]=string17[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string16);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            al_play_sample_instance(sound3_instance);
            al_rest(random_rest);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    for(int f=0; f < sizeof(string18); f++)
    { //Linha 3, card 6
        stringx[f]=string18[f];
        stringx[f+1] = '\0';
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text1y, ALLEGRO_ALIGN_LEFT, string16);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text2y, ALLEGRO_ALIGN_LEFT, string17);
        al_draw_text(font1, al_map_rgb(255, 255, 255), textx, text3y, ALLEGRO_ALIGN_LEFT, stringx);
        al_flip_display();

        if(skip_request == false)
        {
            float random_rest = rand()%10;
            random_rest /=40.0;
            if(string18[f]=='.')
                random_rest+=2;
            al_play_sample_instance(sound3_instance);
            al_rest(random_rest);
        }

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 7
    lore_fade_in_img("assets/img/lore7.bmp", fadespeed, lore_img_posx, lore_img_posy);
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<40)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 8
    lore_fade_in_img("assets/img/lore8.bmp", fadespeed, lore_img_posx, lore_img_posy);
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<40)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(timing==5)
            al_play_sample_instance(sound5_instance); //Efeito sonoro do carinha escorregando

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 9
    lore_fade_in_img("assets/img/lore9.bmp", fadespeed, lore_img_posx, lore_img_posy);
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<40)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    //************************************************************************************************LORE CARD 10 (animacao)
    skip_request = false;
    char imagepath[] = "assets/img/lore10/lore10__000000.bmp";
    //char num[4] = "000"; //Exemplo do formato da string com o numero da imagem
    imagem1 = al_load_bitmap(imagepath);
    lore_fade_in_img(imagepath, fadespeed, lore_img_posx, lore_img_posy); //Fade in na imagem original
    al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
    for(int index = 0; index < 271; index++)
    {
        if(skip_request)
            index = 270; //Vai para a utima imagem (pula a animação)

        sprintf(imagepath, "assets/img/lore10/lore10__000%03d.bmp", index);
        imagem1 = al_load_bitmap(imagepath);
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_bitmap(imagem1, lore_img_posx, lore_img_posy, 0);
        al_flip_display();

        //Permite o fechamento antecipado do jogo, durante a lore inicial
        if (closed)
            return 0;
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            skip_request = true;
            al_play_sample_instance(sound6_instance);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        al_rest(0.06666666);
    }
    timing = 0;
    continua = false;
    while(!closed && !continua && timing<20)
    { //Espera até que o usuario digite alguma tecla (ou até que x tempo passe)
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            continua = togglebool(continua);
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        timing+=1;
        al_rest(0.1);
    }
    fade_to_black(10);
    al_stop_sample_instance(music1_instance);
    return 0;
}
int init_menu()
{
    int fade = 0;

    //Abre as fontes para os ponteiros de fonte
    font1 = al_load_font("undet_tit_fonte.otf", 45, 0);
    font2 = al_load_ttf_font("gomarice_shmup_in_the_zone.ttf", 20, 0);
    font3 = al_load_font("undet_tit_fonte.otf", 28, 0);

    music1 = al_load_sample("assets/music/epic_background_loop.ogg");
    music1_instance = al_create_sample_instance(music1);
    al_set_sample_instance_playmode(music1_instance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(music1_instance, al_get_default_mixer());
    al_play_sample_instance(music1_instance);

    //Não é preciso fazer o loop com o Z e ENTER porque essa rotina é fechada quando isso acontece
    char *frasealeatoria[9] =
    {
        "definitely not undertails",
        "177013",
        "cuidado: vose cringe",
        "never underestimate your inabilities",
        "don't be afraid of failure. be sure of it.",
        "floor gang",
        "also try terraria",
        "reeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
        "so it begins"
    };

    int frasen;
    frasen = rand()%9;

    while(!closed)
    {
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_sample_instance(music1_instance);
            closed=true; //Permite o fechamento da janela
            return -1;
        }

        if(event.keyboard.keycode == ALLEGRO_KEY_Z || event.keyboard.keycode == ALLEGRO_KEY_ENTER)
        {
            al_stop_sample_instance(music1_instance);
            //Fim da funçao
            return 0;
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
        al_rest(0.1);
        if (fade < 385)
            fade+=3;

        al_clear_to_color(al_map_rgb(0, 0, 0)); //Colore a tela de preto
        al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/2-20, ALLEGRO_ALIGN_CENTRE, "Not Undertale");
        al_draw_text(font3, al_map_rgb(255, 0, 0), 405, 223, ALLEGRO_ALIGN_CENTRE, "#");
        al_draw_text(font2, al_map_rgb(fade/3, fade/3, fade/3), LARGURA_TELA/2, (3*ALTURA_TELA)/4-20, ALLEGRO_ALIGN_CENTRE, "[Press Z or Enter]");
        al_draw_text(font2, al_map_rgb(fade/8, fade/8, fade/8), LARGURA_TELA/2, (3*ALTURA_TELA)/4-90, ALLEGRO_ALIGN_CENTRE, frasealeatoria[frasen]);
        al_flip_display(); //Atualiza a tela
    }
    return 0;
}
int encerra(int flag)
{ //Função de encerramento (quando o X da janela é clicado)
    switch(flag)
    {
        case 0:
        {

            font1 = al_load_font("undet_tit_fonte.otf", 20, 0);
            al_draw_text(font1, al_map_rgb(255, 255, 255), 5, 5, ALLEGRO_ALIGN_LEFT, "Exiting...");

            al_set_window_title(janela, "See you next time...");
            fade_to_black(1);
        }
        break;

        case -1:
        {
            error_msg("Nao foi possivel inicializar a biblioteca Allegro!");
        }
            break;

        case -2:
        {
            error_msg("Nao foi possivel inicializar o addon de fontes do Allegro!");
        }
            break;

        case -3:
        {
            error_msg("Nao foi possivel inicializar o sistema de som!");
        }
            break;

        case -4:
        {
            error_msg("Nao foi possivel inicializar os codecs de audio!");
        }
            break;

        case -5:
        {
            error_msg("Nao foi possivel recuperar dados do teclado (permissões)!");
        }
            break;
        case -6:
        {
            error_msg("Nao foi possivel iniciar o addon de imagens (permissões)!");
        }
            break;
    }

    //Desalocaçao de tudo
    ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    al_append_path_component(path, "resources");
    al_change_directory(al_path_cstr(path, '/'));
    al_destroy_path(path);
    // Desalocação da fonte e da janela
    //al_destroy_audio_stream(music1);
    //al_destroy_event_queue(queue);
    al_destroy_display(janela);

    return 0;
}
int instruction_manuals()
{ //Mostra o manual de instruçoes, com um fade

    for (int alfa = 0; alfa <= 200; alfa += 10) //Velocidade do fade
    {
        //Permite fechar o jogo durante o fade
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return 0;
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);

        al_clear_to_color(al_map_rgb(0, 0, 0)); //Imprime um quadrado preto no fundo
        font1 = al_load_font("nundet_m.ttf", 36, 0);
        al_draw_text(font1, al_map_rgb(alfa, alfa, alfa), LARGURA_TELA/2, ALTURA_TELA/4-50, ALLEGRO_ALIGN_CENTER, "-- Instructions --");
        font1 = al_load_font("nundet_m.ttf", 30, 0);
        al_draw_text(font1, al_map_rgb(alfa, alfa, alfa), 125, ALTURA_TELA/4+30*0, ALLEGRO_ALIGN_LEFT, "[Z or ENTER] - Select/Confirm/Use");
        al_draw_text(font1, al_map_rgb(alfa, alfa, alfa), 125, ALTURA_TELA/4+30*1, ALLEGRO_ALIGN_LEFT, "[X or SHIFT] - Esc");
        al_draw_text(font1, al_map_rgb(alfa, alfa, alfa), 125, ALTURA_TELA/4+30*2, ALLEGRO_ALIGN_LEFT, "[C or CTRL] - Inventory");
        al_draw_text(font1, al_map_rgb(alfa, alfa, alfa), 125, ALTURA_TELA/4+30*3, ALLEGRO_ALIGN_LEFT, "[ESC] - Quit game");
        al_draw_text(font1, al_map_rgb(alfa, alfa, alfa), 125, ALTURA_TELA/4+30*4, ALLEGRO_ALIGN_LEFT, "When your HP is 0, you become a furry.");
        font1 = al_load_font("gomarice_shmup_in_the_zone.ttf", 12, 0);
        al_draw_text(font1, al_map_rgb(alfa, alfa, alfa), LARGURA_TELA/2, 15*ALTURA_TELA/16, ALLEGRO_ALIGN_CENTER, "This is NOT UNDERTALE  - v_dev - by Ruim mas Time (2021)");

        al_flip_display();
        al_rest(0.005); // Não é necessário caso haja controle de FPS
    }

    event.type = 0;

    while(!closed)
    {
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }

       if(event.type == ALLEGRO_EVENT_KEY_UP)
        {
            event.keyboard.keycode = 0;
            event.type = 0;
            al_stop_samples();
            return 0;
        }

        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    return 0;
}
void u(int u)
{ //PRO GAMER SOUND EFFECTS
    char pathc[30];
    snprintf (pathc, 30, "%s%d%s", "assets/sound effects/u", u, ".ogg");
    //printf("%s\n", pathc);

    if(u%2)
    {
        sound1 = al_load_sample(pathc);
        sound1_instance = al_create_sample_instance(sound1);
        al_set_sample_instance_playmode(sound1_instance, ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(sound1_instance, al_get_default_mixer());
        al_play_sample_instance(sound1_instance);
    }
    else
    {
        sound2 = al_load_sample(pathc);
        sound2_instance = al_create_sample_instance(sound2);
        al_set_sample_instance_playmode(sound2_instance, ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(sound2_instance, al_get_default_mixer());
        al_play_sample_instance(sound2_instance);
    }

}
int name_confirm(bool* chosen)
{
    al_flush_event_queue(queue); //Limpa a queue

    bool pressed = false;
    imagem1 = al_load_bitmap("assets/img/areusure.bmp");

    ALLEGRO_BITMAP* btp1 = al_get_target_bitmap();
    ALLEGRO_BITMAP* btp2 = al_create_bitmap(LARGURA_TELA, ALTURA_TELA);
    font2 = al_load_font("nundet_m.ttf", 28, 0);
    font3 = al_load_font("nundet_m.ttf", 24, 0);
    font4 = al_load_font("nundet_m.ttf", 16, 0);
    font5 = al_load_font("nundet_m.ttf", 16, 0);

    bool cursorpos = 0;
    int size = 5;
    bool ilegal = false;
    bool escolheu = false;
    bool tried = false;
    if(strlen(heroname)==1 || allCharactersSame(heroname))
        ilegal = true;

    while(!closed)
    {
        //textout_hack(imagem2, font2, LARGURA_TELA/2, ALTURA_TELA/2, (rand()%100)/100, heroname, 1);
        if(size < 100)
        {
            font1 = al_load_font("nundet_m.ttf", size, 0);
                font5 = al_load_font("nundet_m.ttf", (int)(size/6.25), 0);
            size++;
        }
        al_set_target_bitmap(btp1);
        al_clear_to_color(al_map_rgb(0,0,0));
         //Imprime/atualiza a string com o nome do personagem
        float random_rot = ((rand() % (100 + 101) -100)/100.0)*ALLEGRO_PI/-200;// + ALLEGRO_PI/-100 ; //rotaçao aleatoria entre -1 e 1
        if(rmt)
        {
            al_draw_text(font5, al_map_rgb(200,0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, "[RUIM MAS TIME]");
            al_draw_text(font1, al_map_rgb(200, 0, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, heroname);
        }
        else
            if(ultra_gamer)
            {

            }
            else
                if(pro_gamer)
                {
                    al_draw_text(font5, al_map_rgb(200,200, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, "[GOLD]");
                    al_draw_text(font1, al_map_rgb(200, 200, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, heroname);
                }
                else
                    if(gamer)
                    {
                        int color1 = rand()%200;
                        int color2 = rand()%200;
                        int color3 = rand()%200;
                        al_draw_text(font5, al_map_rgb(color1,color2, color3), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, "[Dew]XX");
                        al_draw_text(font1, al_map_rgb(color1, color2, color3), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, heroname);
                    }
                    else
                        if(pro)
                        {
                            al_draw_text(font5, al_map_rgb(0,200, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, "[Dew]");
                            al_draw_text(font1, al_map_rgb(0, 200, 0), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, heroname);
                        }
                        else
                            al_draw_text(font1, al_map_rgb(200, 200, 200), LARGURA_TELA/2, ALTURA_TELA/2, ALLEGRO_ALIGN_CENTER, heroname);

        al_draw_rotated_bitmap(btp1,LARGURA_TELA/2,ALTURA_TELA/2,LARGURA_TELA/2,ALTURA_TELA/2,random_rot,0);
        //al_set_target_bitmap(btp2);
        al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/6, ALLEGRO_ALIGN_CENTER, "Are you sure about that?");
        if(strlen(heroname)==1)
            al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTER, "You call that a gamer?");
            else
                if(allCharactersSame(heroname))
                    al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTER, "Not very creative...");
                else
                    if(rmt)
                        al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTER, "N-nice name...");
                    else
                        if(pro_gamer)
                            al_draw_text(font4, al_map_rgb(255, 255, 0), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTER, "Gold plated.");
                        else
                            if(gamer)
                                al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTER, "Quickscoped.");
                            else
                                if(pro)
                                    al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTER, "Good taste.");
                                else
                                    al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, ALTURA_TELA/4, ALLEGRO_ALIGN_CENTER, "Nice name bro.");
        if(tried)
            al_draw_text(font4, al_map_rgb(180, 0, 0), LARGURA_TELA/2, ALTURA_TELA/4+25, ALLEGRO_ALIGN_CENTER, "(illegal name)");
        if(cursorpos)
        {
            al_draw_text(font2, al_map_rgb(255, 255, 0), LARGURA_TELA*.3, ALTURA_TELA*.8, ALLEGRO_ALIGN_CENTER, "YES");
            al_draw_text(font3, al_map_rgb(255, 255, 255), LARGURA_TELA*.7, ALTURA_TELA*.8, ALLEGRO_ALIGN_CENTER, "NO");
        }
        else
        {
            al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA*.3, ALTURA_TELA*.8, ALLEGRO_ALIGN_CENTER, "YES");
            al_draw_text(font3, al_map_rgb(255, 255, 0), LARGURA_TELA*.7, ALTURA_TELA*.8, ALLEGRO_ALIGN_CENTER, "NO");
        }

        al_draw_bitmap(btp1, 0,0,0);
        al_flip_display();

        if(pressed == false)
        {
            switch(event.keyboard.keycode)
            {
                case ALLEGRO_KEY_RIGHT:
                {
                    cursorpos = togglebool(cursorpos);
                    pressed = true;
                    al_play_sample_instance(sound3_instance);
                }
                break;
                case ALLEGRO_KEY_LEFT:
                {
                    cursorpos = togglebool(cursorpos);
                    pressed = true;
                    al_play_sample_instance(sound3_instance);
                }
                break;
                case ALLEGRO_KEY_ENTER:
                {
                    pressed = true;
                    if(ilegal && cursorpos)
                    {
                        tried = true;
                        al_play_sample_instance(sound5_instance);
                    }

                    if(cursorpos && !ilegal)
                    { //Se o cursor estiver sobre YES
                        *chosen = true;
                        al_destroy_bitmap(btp1);
                        al_destroy_bitmap(btp2);
                        event.keyboard.keycode = 0;
                        escolheu = true;
                        al_stop_sample_instance(music1_instance);
                    }
                    else
                        if(!cursorpos)
                        { //Se o cursor estiver sobre no
                            *chosen = false;
                            al_destroy_bitmap(btp1);
                            al_destroy_bitmap(btp2);
                            event.keyboard.keycode = 0;
                            escolheu = true;
                            al_play_sample_instance(sound4_instance);
                            fade_to_black(5);
                        }
                }
                break;
                default:
                {
                    event.keyboard.keycode = 0;
                }
                break;
            }
            if(escolheu)
            {
                escolheu = false;
                return cursorpos;
            }
        }
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        { //Quando a tecla levantar
            event.keyboard.keycode = 0;
            event.type = 0;
            pressed = false;
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);

        //Gera um frame novo
        al_flip_display();
        al_rest(0.03); //Trava o fps
    }
    return 0;
}
int choose_name(bool* chosen)
{ //Escolhe o nome do gamer

    //Variaveis gamers
    bool pressed = false;
    bool auxe = false;
    bool confirm_name = false;

    //Controle de easter eggs
    bool nword = false;
    bool skyrim = false;
    bool nome_curto = false;
    bool nome_so_um_char = false;
    bool npode = false;
    int count = 0;
    bool burro = false;
    bool mais_burro = false;
    bool continua_burro = false;
    bool not_gamer = false;

    int ui = 1;
    int offset = 0;

    int i=0;
    int j=0;

    POS name_selector = {0, 0}; //x , y
    char aux[2] = {'a', '\0'};

    char matrix[4][7] =
    { //Matriz para a seleção de nome do personagem
        {'A','B','C','D','E','F','G'},
        {'H','I','J','K','L','M','N'},
        {'O','P','Q','R','S','T','U'},
        {'V','W','X','Y','Z','1','2'}
    };

    font1 = al_load_font("nundet_m.ttf", 40, 0);
    font2 = al_load_font("nundet_m.ttf", 35, 0);
    font3 = al_load_font("nundet_m.ttf", 50, 0);
    font4 = al_load_font("nundet_m.ttf", 25, 0);

    sound3 = al_load_sample("assets/sound effects/menu_type.ogg");
    sound3_instance = al_create_sample_instance(sound3);
    al_set_sample_instance_playmode(sound3_instance, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(sound3_instance, al_get_default_mixer());

    sound4 = al_load_sample("assets/sound effects/u_activate.wav");
    sound4_instance = al_create_sample_instance(sound4);
    al_set_sample_instance_playmode(sound4_instance, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(sound4_instance, al_get_default_mixer());

    sound5 = al_load_sample("assets/sound effects/sv7.wav");
    sound5_instance = al_create_sample_instance(sound5);
    al_set_sample_instance_playmode(sound5_instance, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(sound5_instance, al_get_default_mixer());

    sound6 = al_load_sample("assets/sound effects/broken_legs.ogg");
    sound6_instance = al_create_sample_instance(sound6);
    al_set_sample_instance_playmode(sound6_instance, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(sound6_instance, al_get_default_mixer());

    al_flush_event_queue(queue); //Limpa a queue
    while(!closed)
    {
        al_clear_to_color(al_map_rgb(0, 0, 0)); //Colore a tela de preto
        if(skyrim)
        { //Se as duas strings forem iguais
            if(!auxe)
            {
                auxe = togglebool(auxe);
            }
            imagem1 = al_load_bitmap("assets/img/whoru.bmp");
            al_draw_tinted_bitmap(imagem1, al_map_rgba(150, 150, 150, 150), 0, 0, 0);
        }
        else
            if(nword)
            {
                if(!auxe)
                { //Apenas uma vez
                    sound7 = al_load_sample("assets/sound effects/niba.ogg");
                    sound7_instance = al_create_sample_instance(sound7);
                    al_set_sample_instance_playmode(sound7_instance, ALLEGRO_PLAYMODE_ONCE);
                    al_attach_sample_instance_to_mixer(sound7_instance, al_get_default_mixer());
                    al_play_sample_instance(sound7_instance);
                    imagem1 = al_load_bitmap("assets/img/nord.bmp");
                    auxe = togglebool(auxe);
                }
                al_draw_tinted_bitmap(imagem1, al_map_rgba(100, 100, 100, 100), 0, 0, 0);
            }
            else
                if(npode)
                {
                    if(!auxe)
                    {
                        imagem1 = al_load_bitmap("assets/img/npode.bmp");
                    }
                    al_draw_tinted_bitmap(imagem1, al_map_rgba(100, 100, 100, 100), 0, 0, 0);
                }

        al_draw_text(font1, al_map_rgb(245, 245, 245), LARGURA_TELA/2, 35, ALLEGRO_ALIGN_CENTER, "Name the fallen gamer."); //Imprime o titulo da janela
        al_draw_text(font3, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 105, ALLEGRO_ALIGN_CENTER, heroname); //Imprime/atualiza a string com o nome do personagem

        //Subtitulo do nome - e possiveis variantes
        if(!npode)
            if(pro_gamer)
            {
                if(!rmt)
                {
                    al_draw_text(font3, al_map_rgb(255, 255, 0), LARGURA_TELA/2, 105, ALLEGRO_ALIGN_CENTER, "[GOLD]                             "); //Easter egg gold
                }
                else
                    al_draw_text(font3, al_map_rgb(255, 0, 0), LARGURA_TELA/2, 105, ALLEGRO_ALIGN_CENTER, "[TIME]                             "); //Easter egg gold + RMT

                if(burro)
                    al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 150, ALLEGRO_ALIGN_CENTER, "may be overkill..."); //Nome lotado de caracteres + GOLD
            }
            else
            {
                if(burro)
                {
                    if(mais_burro)
                    {
                        if(continua_burro)
                            al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 150, ALLEGRO_ALIGN_CENTER, "Stop it... Get some help...");
                        else
                            al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 150, ALLEGRO_ALIGN_CENTER, "You only need 6 characters...");
                    }
                    else
                        al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 150, ALLEGRO_ALIGN_CENTER, "Is becoming too powerful...");
                }

                if(gamer)
                {
                    al_draw_text(font3, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 105, ALLEGRO_ALIGN_CENTER, "Xx               xX");
                    if(pro)
                        al_draw_text(font3, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 105, ALLEGRO_ALIGN_CENTER, "[Dew]                                      ");
                }
                else
                    if(pro)
                        al_draw_text(font3, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 105, ALLEGRO_ALIGN_CENTER, "[Dew]                            ");
            }
        else
            al_draw_text(font4, al_map_rgb(255, 255, 255), LARGURA_TELA/2, 150, ALLEGRO_ALIGN_CENTER, "Are you a furry or something?");

        al_draw_text(font2, al_map_rgb(150, 150, 150), LARGURA_TELA/2, 4*ALTURA_TELA/5, ALLEGRO_ALIGN_CENTER, "[Q] Quit     [X] Backspace     [ENTER] Done");


        for (int i = 0; i < 7 && !closed; i++)
        {
            for (int j = 0; j < 4 && !closed; j++)
            {
                offset = ((rand() - RAND_MAX / 2)%2);
                aux[0] = matrix[j][i];

                if(name_selector.posicao_y == j && name_selector.posicao_x == i) //Se for igual a linha e coluna
                {
                    if(matrix[j][i] == '1')
                        al_draw_text(font1, al_map_rgb(245, 245, 0), 89+78*i, 200+40*j, ALLEGRO_ALIGN_CENTER, "PRO");
                    else
                        if(matrix[j][i] == '2')
                            al_draw_text(font1, al_map_rgb(245, 245, 0), 89+78*i, 200+40*j, ALLEGRO_ALIGN_CENTER, "GAMER");
                        else
                            al_draw_text(font1, al_map_rgb(255, 245, 0), 89+78*i, 200+40*j, ALLEGRO_ALIGN_CENTER, aux);
                }
                else
                    if(matrix[j][i] == '1')
                        al_draw_text(font1, al_map_rgb(245, 245, 245), 89+78*i, 200+40*j, ALLEGRO_ALIGN_CENTER, " ");
                    else
                        if(matrix[j][i] == '2')
                            al_draw_text(font1, al_map_rgb(245, 245, 245), 89+78*i, 200+40*j, ALLEGRO_ALIGN_CENTER, " ");
                        else
                            al_draw_text(font1, al_map_rgb(245, 245, 245), 89+78*i+offset, 200+40*j+offset, ALLEGRO_ALIGN_CENTER, aux); //Se for qualquer char comum

                if(pressed == false)
                {
                    switch(event.keyboard.keycode)
                    {
                        case ALLEGRO_KEY_RIGHT:
                        {
                            if(name_selector.posicao_x == 6)
                                name_selector.posicao_x = 0;
                            else
                                name_selector.posicao_x++;
                            pressed = true;
                            al_play_sample_instance(sound3_instance);
                        }
                        break;
                        case ALLEGRO_KEY_LEFT:
                        {
                            if(name_selector.posicao_x == 0)
                                name_selector.posicao_x = 6;
                            else
                                name_selector.posicao_x--;
                            pressed = true;
                            al_play_sample_instance(sound3_instance);
                        }
                        break;
                        case ALLEGRO_KEY_UP:
                        {
                            if(name_selector.posicao_y == 0)
                                name_selector.posicao_y = 3;
                            else
                                name_selector.posicao_y--;
                            pressed = true;
                            al_play_sample_instance(sound3_instance);
                        }
                        break;
                        case ALLEGRO_KEY_DOWN:
                        {
                            if(name_selector.posicao_y == 3)
                                name_selector.posicao_y = 0;
                            else
                                name_selector.posicao_y++;
                            pressed = true;
                            al_play_sample_instance(sound3_instance);
                        }
                        break;
                        case ALLEGRO_KEY_Z:
                        {
                            switch(matrix[name_selector.posicao_y][name_selector.posicao_x])
                            {
                                case '1':
                                {
                                    pro = togglebool(pro);
                                    u(ui);
                                    if(ui < 12)
                                    {
                                        ui++;
                                        pro_gamer = false;
                                    }
                                    else
                                    {
                                        ui = 1;
                                        al_play_sample_instance(sound4_instance);
                                        pro_gamer = true;
                                        pro = false;
                                        gamer = false;
                                    }
                                }
                                break;
                                case '2':
                                {
                                    gamer = togglebool(gamer);
                                    u(ui);
                                    if(ui < 12)
                                    {
                                        ui++;
                                        pro_gamer = false;
                                    }
                                    else
                                    {
                                        ui = 1;
                                        al_play_sample_instance(sound4_instance);
                                        pro_gamer = true;
                                        pro = false;
                                        gamer = false;
                                    }
                                }
                                break;
                                default:
                                {
                                    if(strlen(heroname) < 6)
                                    {
                                        heroname[(strlen(heroname))] = matrix[name_selector.posicao_y][name_selector.posicao_x];
                                        heroname[(strlen(heroname))+1] = '\0';
                                        pressed = true;
                                        al_play_sample_instance(sound3_instance);

                                        //Easter egg checks
                                        if(!strcmp(heroname, "SKYRIM"))
                                            skyrim = true;
                                        else
                                            skyrim = false;
                                        if(!strcmp(heroname, "NIGGER"))
                                            nword = true;
                                        else
                                            nword = false;
                                        if(!strcmp(heroname, "CHAD"))
                                        {
                                            ultra_gamer = true;
                                            al_play_sample_instance(sound4_instance);
                                        }
                                        else
                                            if(!rmt)
                                                ultra_gamer = false;

                                        if(!strcmp(heroname, "RUIVA")
                                           || !strcmp(heroname, "LOIRA")
                                           || !strcmp(heroname, "OTOME")
                                           || !strcmp(heroname, "YANDER")
                                           || !strcmp(heroname, "GOTICA")
                                           || !strcmp(heroname, "MORENA")
                                           || !strcmp(heroname, "EDUCAD")
                                           || !strcmp(heroname, "RMLOLI"))
                                        {
                                            rmt = true;
                                            al_play_sample_instance(sound4_instance);
                                        }
                                        else
                                            rmt = false;
                                            ultra_gamer = false;
                                        if(!strcmp(heroname, "HITLER")
                                           || !strcmp(heroname, "UWU")
                                           || !strcmp(heroname, "NAZI")
                                           || !strcmp(heroname, "TREVI")
                                           || !strcmp(heroname, "EMI")
                                           || !strcmp(heroname, "FLOWEY")
                                           || !strcmp(heroname, "CHARA")
                                           || !strcmp(heroname, "FRISK")
                                           || !strcmp(heroname, "TORIEL")
                                           || !strcmp(heroname, "ASGORE")
                                           || !strcmp(heroname, "UNDYNE")
                                           || !strcmp(heroname, "ALPHYS")
                                           || !strcmp(heroname, "TORIEL")
                                           || !strcmp(heroname, "BITCH")
                                           || !strcmp(heroname, "FLUFFY")
                                           || !strcmp(heroname, "YIFFIN")
                                           || !strcmp(heroname, "FLUFFY"))
                                            npode = true;
                                        else
                                            npode = false;

                                        auxe = false;
                                    }
                                    else //Caso o nome do personagem esteja cheio ja
                                    {
                                        al_play_sample_instance(sound5_instance); //Som de erro
                                        burro = true;
                                        count++;
                                        if(count> 10)
                                            mais_burro = true;
                                        if (count> 25)
                                            continua_burro = true;
                                    }
                                }
                                break;
                            }

                            pressed = true;
                        }
                        break;
                        case ALLEGRO_KEY_X:
                        {
                            burro = false; //O cara n é mais burro porque o tamanho do nome ja nao é mais o maximo
                            mais_burro = false;
                            continua_burro = false;
                            count = 0;
                            heroname[(strlen(heroname))-1] = '\0';
                            pressed = true;
                            al_play_sample_instance(sound6_instance);

                            //Easter egg check
                            if(!strcmp(heroname, "SKYRIM"))
                                skyrim = true;
                            else
                                skyrim = false;
                            if(!strcmp(heroname, "NIGGER"))
                                nword = true;
                            else
                                nword = false;
                            if(!strcmp(heroname, "RUIVA")
                               || !strcmp(heroname, "LOIRA")
                               || !strcmp(heroname, "OTOME")
                               || !strcmp(heroname, "YANDER")
                               || !strcmp(heroname, "GOTICA")
                               || !strcmp(heroname, "MORENA")
                               || !strcmp(heroname, "EDUCAD")
                               || !strcmp(heroname, "COREAN")
                               || !strcmp(heroname, "RMLOLI"))
                            {
                                rmt = true;
                                al_play_sample_instance(sound4_instance);
                            }
                            else
                                rmt = false;
                            if(!strcmp(heroname, "HITLER")
                               || !strcmp(heroname, "NAZI")
                               || !strcmp(heroname, "TREVI")
                               || !strcmp(heroname, "EMI")
                               || !strcmp(heroname, "FLOWEY")
                               || !strcmp(heroname, "CHARA")
                               || !strcmp(heroname, "FRISK")
                               || !strcmp(heroname, "TORIEL")
                               || !strcmp(heroname, "ASGORE")
                               || !strcmp(heroname, "UNDYNE")
                               || !strcmp(heroname, "ALPHYS")
                               || !strcmp(heroname, "TORIEL")
                               || !strcmp(heroname, "BITCH"))
                                npode = true;
                            else
                                npode = false;

                            auxe = false;
                        }
                        break;
                        case ALLEGRO_KEY_Q:
                        {
                            closed = true;
                            pressed = true;
                        }
                        break;
                        case ALLEGRO_KEY_ENTER:
                        {
                            if(!npode && strlen(heroname) > 0 && !not_gamer)
                                confirm_name = true;
                            event.keyboard.keycode = 0;
                            event.type = 0;
                            pressed = true;
                        }
                        break;
                    }

                    //Confirma o nome do char******************************************************************************************************************************
                    if(confirm_name)
                    {
                        confirm_name = false;
                        //*chosen = true;
                        return 0;
                    }
                }

                //Eu prefiro não comentar o que isso faz...
                /*if(closed)
                {
                    //Nao apagar esse comentario (memorias)
                    i = 45453;
                    j = 435434;
                }*/

            }
        }
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            al_stop_samples();
            closed = true; //Permite o fechamento da janela
            return -1;
        }
        if(event.type == ALLEGRO_EVENT_KEY_UP)
        { //Quando a tecla levantar
            event.keyboard.keycode = 0;
            event.type = 0;
            pressed = false;
        }

        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);

        //Gera um novo frame
        al_flip_display();
    }
}
int main_menu()
{
    al_clear_to_color(al_map_rgb(0, 0, 0)); //Colore a tela de preto
    al_flip_display(); //Atualiza a tela

    music1 = al_load_sample("assets/music/menu.ogg");
    music1_instance = al_create_sample_instance(music1);
    al_set_sample_instance_gain(music1_instance, 0.5); //Ajusta o volume da musica
    al_set_sample_instance_playmode(music1_instance, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(music1_instance, al_get_default_mixer());
    al_play_sample_instance(music1_instance);

    instruction_manuals();
    if (closed) //Permite o encerramento antecipado do programa
        return 0;

    bool chosen = false;

    while(!chosen)
    {
        choose_name(&chosen);
        al_rest(0.5);
        event.keyboard.keycode = 0;
        name_confirm(&chosen);
        if (closed) //Permite o encerramento antecipado do programa
            return 0;
    }
    fade_to_white(1);
    al_rest(1);
    while(1)
        al_rest(1);


    while(!closed)
    {
        //printf("rodou aq");
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
        {
            closed=true; //Permite o fechamento da janela
            return 0;
        }
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }
    return 0;
}
void push_dirorder(int* arr, int newdir)
{ //(dir_primaria, dir secundaria)
    if(arr[0] != newdir && arr[1] != newdir)
        if(arr[0] == 0)
        {
            arr[0] = newdir; //Se a pos 0 for zero, a posiçao informada é primaria
            return;
        }
        else //Se não, é secundaria
        {
            arr[1] = newdir;
            return;
        }
    else
        return;

    return;
}
void pop_dirorder(int* arr, int val)
{
    if(arr[0] == val)
    {
        arr[0] = arr[1];
        return;
    }
    else
        if(arr[1] == val)
        {
            arr[1] = 0;
            return;
        }
        else
            return;

    if(arr[0] == 0)
        arr[0] = arr[1];
    return;
}
void caido()
{
    MOB_PROP pepe;
    pepe.h = 19;
    pepe.w = 29;
    pepe.posx = 0;
    pepe.posy = 0;
    pepe.velx = 0;
    pepe.vely = 0;
    pepe.wx = 0;
    pepe.wy = 0;
    pepe.movespeed = 5;
    pepe.animation_delay = 6; //Quanto menor, mais rapido
    pepe.dir = 2; //Personagem começa olhando para baixo

    int framecounter = 0;
    int anim_aux = 0;

    int first_pressed = 1; //1-
    bool alterna = false;
    bool alterna_vert = false;

    int dirorder[3] = {0,0};

    ALLEGRO_BITMAP* pepe_sprite = al_load_bitmap("assets/img/pepe_char.bmp");
    ALLEGRO_BITMAP* bg = al_load_bitmap("assets/img/test.bmp");

    while(!closed)
    {
        if (event.keyboard.display != janela) //Se a janela for minimizada, reseta a velocidade
        {
            //out of focus
            pepe.velx = 0;
            pepe.vely = 0;
            pepe.dir = 2; //Baixo parado é a direção padrao
            first_pressed = 2;
            pop_dirorder(dirorder, 1);
            pop_dirorder(dirorder, 2);
            pop_dirorder(dirorder, 3);
            pop_dirorder(dirorder, 4);
            printf("Out of focus!\n");
        }

        while (!al_is_event_queue_empty(queue))
        {
            al_wait_for_event(queue, &event);

            if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || closed == true)
            { //Se a janela fechar
                al_stop_samples();
                closed = true; //Permite o fechamento da janela
                return;
            }

            switch(event.type)
            {
                case ALLEGRO_EVENT_KEY_DOWN:
                    switch(event.keyboard.keycode)
                    {
                        case ALLEGRO_KEY_UP:
                            pepe.dir = 1;
                            push_dirorder(dirorder, pepe.dir);
                            pepe.vely -= pepe.movespeed;
                            break;
                        case ALLEGRO_KEY_DOWN:
                            pepe.dir = 2;
                            push_dirorder(dirorder, pepe.dir);
                            pepe.vely += pepe.movespeed;
                            break;
                        case ALLEGRO_KEY_LEFT:
                            pepe.dir = 3;
                            push_dirorder(dirorder, pepe.dir);
                            pepe.velx -= pepe.movespeed;
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            pepe.dir = 4;
                            push_dirorder(dirorder, pepe.dir);
                            pepe.velx += pepe.movespeed;
                            break;

                        case ALLEGRO_KEY_ESCAPE: //Fecha o jogo
                            closed = true;
                            break;
                    }
                    break;
                case ALLEGRO_EVENT_KEY_UP:
                    switch(event.keyboard.keycode)
                    {
                        case ALLEGRO_KEY_UP:
                            pepe.vely += pepe.movespeed;
                            pop_dirorder(dirorder, 1);
                            first_pressed = 0;
                            break;
                        case ALLEGRO_KEY_DOWN:
                            pepe.vely -= pepe.movespeed;
                            pop_dirorder(dirorder, 2);
                            first_pressed = 0;
                            break;
                        case ALLEGRO_KEY_LEFT:
                            pepe.velx += pepe.movespeed;
                            pop_dirorder(dirorder, 3);
                            first_pressed = 0;
                            break;
                        case ALLEGRO_KEY_RIGHT:
                            pepe.velx -= pepe.movespeed;
                            pop_dirorder(dirorder, 4);
                            first_pressed = 0;
                            break;
                    }
                    break;
                //break;
            }
        }
        //Ajusta a posiçao do personagem
        printf("Array (%d, %d).\n", dirorder[0], dirorder[1]);
        //printf("Personagem -> V(%d, %d)=%.2f P(%f, %f) Dir=%d - Alterna=%d - Alterna_H=%d.\n", pepe.velx, pepe.vely, sqrt(pow(pepe.velx,2) + pow(pepe.vely,2)), pepe.posx, pepe.posx, pepe.dir, alterna, alterna_vert);
        pepe.posx += pepe.velx;
        pepe.posy += pepe.vely;

        if(framecounter == pepe.animation_delay)
        { //Controle de delay para gerar as animaçoes
            alterna = !alterna;
            framecounter = 0;
            if(alterna)
                alterna_vert = !alterna_vert;
        }

        //Controle de animaçoes do pepe
        switch(pepe.vely)
        {
            case -5: //Em movimento pra cima
                if(pepe.velx == 0)
                { //Nao pode ja estar se movendo em X
                    pepe.wx = 1 + 19 + 19*alterna_vert;
                    pepe.wy = 1 +29*2;
                    pepe.h = 29;
                    pepe.w = 19;
                    pepe.scale_x = 39;
                    pepe.scale_y = 58;
                }
                break;
            case 0: //Caso não esteja andando pra cima
                switch(pepe.velx)
                {
                    case -5: //Indo para a esquerda
                        if(pepe.vely == 0)
                        {
                            pepe.wx = 1 + 15*alterna;
                            pepe.wy = 1 +29*1;
                            pepe.h = 29;
                            pepe.w = 16;
                            pepe.scale_x = 31;
                            pepe.scale_y = 58;
                        }
                        break;
                    case 0: //Caso não esteja andando para o lado - Parado para...
                        switch(pepe.dir)
                        {
                            case 2: //Baixo
                                pepe.wx = 1;
                                pepe.wy = 1;
                                pepe.h = 29;
                                pepe.w = 19;
                                pepe.scale_x = 39;
                                pepe.scale_y = 58;
                                break;
                            case 1: //Cima
                                pepe.wx = 1;
                                pepe.wy = 1 +29*2;
                                pepe.h = 29;
                                pepe.w = 19;
                                pepe.scale_x = 39;
                                pepe.scale_y = 58;
                                break;
                            case 3: //Esq
                                pepe.wx = 1;
                                pepe.wy = 1 +29*1;
                                pepe.h = 29;
                                pepe.w = 15;
                                pepe.scale_x = 31;
                                pepe.scale_y = 58;
                                break;
                            case 4: //Dir
                                pepe.wx =1 + 1*3 + 15*3;
                                pepe.wy = 1 +29*1;
                                pepe.h = 29;
                                pepe.w = 15;
                                pepe.scale_x = 31;
                                pepe.scale_y = 58;
                                break;
                        }
                        break;
                    case 5: //Indo para a direita
                        if(pepe.vely == 0)
                        { //Se ja não tiver um movimento em X
                            pepe.wx =1 + 2 + 15*2 + 15*alterna;
                            pepe.wy = 1 +29*1;
                            pepe.h = 29;
                            pepe.w = 16;
                            pepe.scale_x = 31;
                            pepe.scale_y = 58;
                        }
                        break;
                }
                break;
            case 5: //Em movimento pra baixo
                if(pepe.velx == 0)
                { //Caso não ja tenha um movimento em X
                    pepe.wx = 1 + 19 + 19*alterna_vert;
                    pepe.wy = 1;
                    pepe.h = 29;
                    pepe.w = 19;
                    pepe.scale_x = 39;
                    pepe.scale_y = 58;
                }
                break;
        }

        //Gera um frame novo
        al_clear_to_color(al_map_rgb(0,0,0));
        //al_draw_bitmap(pepe_sprite, pepe.x,pepe.y,0);
        al_convert_mask_to_alpha(pepe_sprite, al_map_rgb(255,0,255)); //Maskeia o truecolor (rosa do bitmap para ter transparencia)
        //al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        //al_draw_bitmap_region(pepe_sprite, 0, 0, 23+1, 30+1, pepe.x, pepe.y, 0);
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_scaled_bitmap(pepe_sprite, pepe.wx, pepe.wy, pepe.w, pepe.h, pepe.posx, pepe.posy, pepe.scale_x, pepe.scale_y, 0);

        al_flip_display();
        framecounter += 1;
        al_rest(0.03); //Trava o fps

    }

    passou(-1);
    al_clear_to_color(al_map_rgb(0,0,0)); //Limpa a tela
    return;
}
int main(void)
{
    eflag = inicializa();
    if(eflag)
    {
        encerra(eflag);
        return -1;
    }

    disclaimer_nundertale();
    if (closed)
    { //Permite o encerramento antecipado do programa
        encerra(eflag);
        return 0;
    }

    init_lore();
    if (closed)
    { //Permite o encerramento antecipado do programa zz
        encerra(eflag);
        return 0;
    }

    init_menu();//Imprime o menu inicial do joguinho (ou fecha tudo)
    if (closed)
    { //Permite o encerramento antecipado do programa
        encerra(eflag);
        return 0;
    }

    main_menu();
    if (closed)
    { //Permite o encerramento antecipado do programa
        encerra(eflag);
        return 0;
    }

    caido();
    if(closed)
    {
        encerra(eflag);
        return 0;
    }

    while(!closed)
    {
        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            closed=true; //Permite o fechamento da janela
        if (!al_is_event_queue_empty(queue))
            al_wait_for_event(queue, &event);
    }

    encerra(0);
    return 0;
}
