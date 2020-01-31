#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define LARGURA 800
#define ALTURA 600
#define FPS 60
#define SPEED 3
#define LARGURA_PLAYER 66
#define ALTURA_PLAYER 68

#define OPENING 0
#define MENU 1
#define OPTIONS 2
#define RANKING 3
#define CREDITS 4
#define HISTORIA 5
#define JOGO 6
#define PAUSE 7
#define STATUS 8
#define INVENTORY 9
#define FALA 10
#define DERROTA 11
#define SCORE_MARK 12
#define VITORIA 13

#define NEW_GAME 1
#define LOAD_GAME 2
#define OPCOES 3
#define SCORE 4
#define CREDITOS 5
#define QUIT 6

#define PAUSE_RESUME 1
#define PAUSE_STATUS 2
#define PAUSE_INVENTORY 3
#define PAUSE_SAVE 4
#define PAUSE_LOAD 5
#define PAUSE_OPTIONS 6
#define PAUSE_QUIT 7

typedef struct {
	SDL_Rect dest, source;
	SDL_Texture *text;
	char *nome;
	unsigned int pontos, vida, dano, armadura, mana;
	int speed_x, speed_y, up, down, left, right, direction;
	int posxmapa, posymapa;
	int acao, soco, magia;
	int magiaeq, espadaeq, elmofeq, peitoralfeq, botafeq;
	int elmoceq, peitoralceq, botaceq;
	int espada, pocaov, pocaom, capacetef, peitoralf, botasf, capacetec, peitoralc, botasc;
} Player; //struct do player

typedef struct {
	SDL_Rect rect, source;
	SDL_Texture *text;
	int direction;
} NPC; //struct de npcs

typedef struct {
	SDL_Texture *text;
	SDL_Rect rect;
	int speed_y;
} Tiro;

typedef struct {
	SDL_Rect rect, source;
	SDL_Texture *text;
	int vida, pontos, dano;
	int speed_x, speed_y;
	int atira;
	Tiro bala;
} Inimigo; //struct dos inimigos

char *alocaChar(int tam);
void desalocChar(char *vet);
SDL_Texture *loadText(SDL_Renderer *r, char *path); //Retorna uma textura do SDL
void fadeIn(SDL_Window *w, SDL_Renderer *r, char *path); //Da efeito de aparecer
void fadeOut(SDL_Window *w, SDL_Renderer *r, char *path); //Da efeito de sumir
void fadeInHistoria(SDL_Window *w, SDL_Renderer *r, char *path, SDL_Rect pos); //Mostra a historia
void printTexto(SDL_Renderer *r, SDL_Color color, TTF_Font *font, SDL_Rect pos, char *frase); //Printa um texto na tela
void fadeInMenu(SDL_Window *w, SDL_Renderer *r, char *path); //Mostra o Menu
void sortRanking(Player *ranking, FILE *pa); // Organiza o ranking
void reescreveArq(Player *ranking, FILE *pa, Player p4);
void animation(SDL_Rect *source, int direction); //Anima a sprite do personagem
int moveInimigo(Player p, Inimigo *i);
int atacarInimigo(Player p, Inimigo *i);
int colisaoMagia(SDL_Rect *magiarect, int magiadirection, Inimigo *i, int *lanca_magia);
int colisaoTiro(Tiro tiro, Player *p, int *atira);

SDL_Window *win = NULL;
SDL_Renderer *render = NULL;
SDL_Event e;
TTF_Font *font = NULL;

int main() {
	int jogando = 1, game_state = OPENING, frame_delay = 1000/FPS, frame_time, frame_start, inicio = 0, i, j, cont = 0, pause_state = 1, contdano = 0, contanima = 0;
	int aperta = 0, game_mode = 0, fala = 0, proxfala, inventcontrol = 1, selecao = 0, lanca_magia = 0, magiadirection = 0, spawn_fantasma = 0, contsoco = 0, falaheroi = 0;
	int state = NEW_GAME, mododejogo = 1, dojogo = 0;
	char *def, *dano, *pontos;
	char *potionh, *potionm, *sword, *helmetl, *helmeti, *chestl, *chesti, *bootsl, *bootsi;
	char *pontuacao;
	char *pontuacaoranking1, *pontuacaoranking2, *pontuacaoranking3;
	pontuacao = alocaChar(5);
	pontuacaoranking1 = alocaChar(5);	pontuacaoranking2 = alocaChar(5);	pontuacaoranking3 = alocaChar(5);
	SDL_Rect potionh_inv, potionm_inv, sword_inv, helmetl_inv, helmeti_inv, chestl_inv, chesti_inv, bootsl_inv, bootsi_inv;
	Player player;
	NPC guardas[4];
	NPC guardasdem[2];
	NPC oraculo;
	Inimigo fantasma;
	Inimigo heroi;
	Inimigo bruxo[7];
	FILE *pa;
	SDL_Texture *moedatext, *menutext, *Backgroundtext, *pausetext, *indicadortext;
	SDL_Rect moedarect;
	Player ranking[4];
	SDL_Rect numeros[3];
	SDL_Rect moedaVoltar;
	SDL_Texture *mapabacktext;
	SDL_Texture *statustext;
	NPC seta;

	player.posxmapa = -3210; player.posymapa = -650;

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	if(TTF_Init() < 0) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	win = SDL_CreateWindow("HumanSlayer", 100, 100, LARGURA, ALTURA, 0);

	if(win == NULL) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	render = SDL_CreateRenderer(win, -1, SDL_RENDERER_PRESENTVSYNC || SDL_RENDERER_ACCELERATED);

	if(render == NULL) {
		printf("Erro: %s\n", SDL_GetError());
		jogando = 0;
	}

	//Inicializa todas as variaveis

	SDL_Color vermelho = {255, 0, 0, 255};
	SDL_Color branco = {255, 255, 255, 255};
	SDL_Color marrom = {92, 51, 23, 255};
	SDL_Rect indicadorrect;	indicadorrect.w = 10;	indicadorrect.h = 10;	indicadorrect.x = 610;	indicadorrect.y = 55;
	SDL_Rect pauserect;	pauserect.w = 200;	pauserect.h = 600;	pauserect.x = (LARGURA - pauserect.w);	pauserect.y = 0; 
	SDL_Rect voltar;	voltar.w = 100;		voltar.x = 100;		voltar.h = 50;		voltar.y = 500;
	SDL_Rect titulorect;	titulorect.w = 400;		titulorect.h = 100;		titulorect.x = (LARGURA - titulorect.w)/2;	titulorect.y = 50;
	SDL_Rect telameio;	telameio.w = 600;	telameio.h = 100;	telameio.x = (LARGURA - telameio.w)/2;	telameio.y = (ALTURA - telameio.h)/2;
	SDL_Rect historiarect; historiarect.w = 800;	historiarect.h = 1480;		historiarect.x = 0;		historiarect.y = 20;
	SDL_Rect voltar_status;	voltar_status.x = 600, voltar_status.y = 500;	voltar_status.w = 25;	voltar_status.h = 25;
	SDL_Rect mapaback;	mapaback.x = -3270;	mapaback.y = -730;	mapaback.w = 5000;	mapaback.h = 2000;
	SDL_Rect player_status, player_status_source, danorect, defrect, vidarect;
	SDL_Rect mapafore; mapafore.x = -3270; mapafore.y = -730; mapafore.w = 5000; mapafore.h = 2000;
	SDL_Rect castelodemonrect;
	SDL_Rect selecao_inv;	selecao_inv.x = 450;	selecao_inv.y = 20;		selecao_inv.h = 25;	selecao_inv.w = 25;
	SDL_Rect iteminvent;	iteminvent.x = 90; iteminvent.y = 380;	iteminvent.h = 200;	iteminvent.w = 200;
	SDL_Texture *itemmostra;
	SDL_Texture *historiatext = loadText(render, "Resources/Historia.png");
	SDL_Texture *mapaforetext = loadText(render, "Resources/Mapafore.png");
	SDL_Texture *castelodemontext = loadText(render, "Resources/Castelo_demon.png");
	SDL_Texture *derrotatext = loadText(render, "Resources/Derrota.png");
	SDL_Texture *scoretext = loadText(render, "Resources/ScoreMark.png");
	SDL_Texture *savetext = loadText(render, "Resources/Save.png");
	SDL_Texture *loadtext = loadText(render, "Resources/Load.png");
	SDL_Texture *coracao = loadText(render, "Resources/Coracao.png");
	SDL_Texture *manahud = loadText(render, "Resources/Manahud.png");
	SDL_Texture *magiatext = loadText(render, "Resources/Boladefogo.png");
	SDL_Texture *part2text = loadText(render, "Resources/Part2.png");
	SDL_Texture *vitoriatext = loadText(render, "Resources/GG.png");
	SDL_Texture *modo = loadText(render, "Resources/modo.png");
	fantasma.text = loadText(render, "Resources/Fantasma.png");
	fantasma.rect.x = player.dest.x;	fantasma.rect.h = 50;	fantasma.rect.y = player.dest.y - fantasma.rect.h;	fantasma.rect.w = 50;
	fantasma.vida = 50;	fantasma.dano = 5;	fantasma.pontos = 50; fantasma.speed_x = 2;	fantasma.speed_y = 2;
	SDL_Rect magiarect;
	SDL_Rect part2rect;
	SDL_Rect rankingsrect[3];	rankingsrect[0].x = 570;	rankingsrect[0].y = 180;	rankingsrect[0].w = 200;	rankingsrect[0].h = 50;	
	SDL_Rect pontrect;
	part2rect.w = 1000;	part2rect.h = 800;	part2rect.x = -85;	part2rect.y = -448;
	SDL_Rect magiasource;	magiasource.h = 34;	magiasource.y = 0;
	SDL_Rect coracaorect[5];
	SDL_Rect coracaosource[5];
	SDL_Rect manarect[5];
	SDL_Rect manasource[5];
	SDL_Rect reidemonrect;
	SDL_Texture *castelo_part1 = loadText(render, "Resources/Castelo1.png");
	SDL_Rect part1;
	SDL_Rect digite;	digite.x = 150;	digite.y = 150;	digite.h = 300;	digite.w = 500;
	SDL_Rect status_capacete, status_peitoral, status_bota, status_espada;
	SDL_Texture *capaceteeq, *peitoraleq, *botaeq, *espadaeq;
	SDL_Texture *guardad = loadText(render, "Resources/guardad.png");
	SDL_Rect guardadrect;	guardadrect.x = 250;	guardadrect.y = 180;	guardadrect.w = 102;	guardadrect.h = 102;
	SDL_Texture *rei_demontext = loadText(render, "Resources/Rei_demon.png");
	SDL_Texture *inventory = loadText (render, "Resources/Inventoy.png");
	pausetext = loadText(render, "Resources/Pause.png");
	statustext = loadText(render, "Resources/Status.png");
	indicadortext = loadText(render, "Resources/Escolhepause.png");
	mapabacktext = loadText(render, "Resources/Backmapa.png");
	oraculo.text = loadText(render, "Resources/Oraculo.png");

	SDL_Texture *falatext;
	seta.text = loadText(render, "Resources/Seta.png");

	guardas[0].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[0].source.x = 34;	guardas[0].source.y = 0;	guardas[0].source.h = 34;	guardas[0].source.w = 34;
	guardas[0].rect.x = -1800; 	guardas[0].rect.y = 500;	guardas[0].rect.h = 102;	guardas[0].rect.w = 112;
	guardas[1].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[1].source.x = 0;	guardas[1].source.y = 0;	guardas[1].source.h = 34;	guardas[1].source.w = 34;
	guardas[1].rect.x = -1800; 	guardas[1].rect.y = 700;	guardas[1].rect.h = 102;	guardas[1].rect.w = 112;
	guardas[2].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[2].source.x = 34;	guardas[2].source.y = 0;	guardas[2].source.h = 34;	guardas[2].source.w = 34;
	guardas[2].rect.x = -3050; 	guardas[2].rect.y = 100;	guardas[2].rect.h = 102;	guardas[2].rect.w = 112;
	guardas[3].text = loadText(render, "Resources/CavaleiroNPC.png");
	guardas[3].source.x = 34;	guardas[3].source.y = 0;	guardas[3].source.h = 34;	guardas[3].source.w = 34;
	guardas[3].rect.x = -2800; 	guardas[3].rect.y = 100;	guardas[3].rect.h = 102;	guardas[3].rect.w = 112;


	font = TTF_OpenFont("Resources/Purisa.ttf", 20);
	numeros[0].x = 200; numeros[0].y = 200;	numeros[0].h = 50;	numeros[0].w = 50;
	numeros[1].x = 200; numeros[1].y = 250;	numeros[1].h = 50;	numeros[1].w = 50;
	numeros[2].x = 200; numeros[2].y = 300;	numeros[2].h = 50;	numeros[2].w = 50;
	ranking[0].dest.x = 300; ranking[0].dest.y = 200;	ranking[0].dest.h = 50;		ranking[0].dest.w = 200;
	ranking[1].dest.x = 300; ranking[1].dest.y = 250;	ranking[1].dest.h = 50;		ranking[1].dest.w = 200;
	ranking[2].dest.x = 300; ranking[2].dest.y = 300;	ranking[2].dest.h = 50;		ranking[2].dest.w = 200;

	player.dano = 10;	player.vida = 100;	player.armadura = 0;	player.pontos = 0;	player.mana = 200;
	player.espada = 0; player.pocaov = 0;	player.pocaom = 0; player.capacetef = 0; player.peitoralf = 0; player.botasf = 0; player.capacetec = 0; player.peitoralc = 0; player.botasc = 0;
	player.acao = 0;	player.soco = 0;	player.magia = 0;
	player.magiaeq = 0; player.espadaeq = 0; player.elmoceq = 0; player.peitoralceq = 0; player.botaceq = 0; player.elmofeq = 0; player.peitoralfeq = 0; player.botafeq = 0;

	coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
	coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
	coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
	coracaosource[3].x = 0;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
	coracaosource[4].x = 0;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;

	manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
	manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
	manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
	manasource[3].x = 0;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
	manasource[4].x = 0;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;

	player.text = loadText(render, "Resources/Player.png");
	player.speed_x = player.speed_y = player.up = player.down = player.left = player.right = 0;
	player.dest.w = 66;	player.dest.h = 102;	player.dest.x = (LARGURA - player.dest.w)/2;	player.dest.y = (ALTURA - player.dest.h)/2;
	player.source.w = 22;	player.source.h = 34;	player.source.x = 0;		player.source.y = 0;
	player.direction = 1;
	castelodemonrect.x = -100 + (player.dest.w/2);	castelodemonrect.y = -1100;	castelodemonrect.w = 1000;	castelodemonrect.h = 2000;
	reidemonrect.y = -750;	reidemonrect.x = 325;	reidemonrect.w = 165;	reidemonrect.h = 255;
	part1.x = -100;		part1.y = -345;		part1.h = 750;		part1.w = 1000;
	oraculo.rect.x = 1300;	oraculo.rect.y = 500;	oraculo.rect.w = 102;	oraculo.rect.h = 102;

	moedarect.w = 25;	moedarect.h = 25;	moedarect.x = 10;	moedarect.y = 270;
	seta.rect.x = 200;	seta.rect.y = 400;	seta.rect.w = 50;	seta.rect.h = 50;

	for(i = 0; i < 7; i++) {
		bruxo[i].text = loadText(render, "Resources/Mago.png");
		bruxo[i].bala.text = loadText(render, "Resources/Magotiro.png");
		bruxo[i].bala.speed_y = 6;
		bruxo[i].atira = 0;
		bruxo[i].vida = 100;
		bruxo[i].pontos = 200;
		bruxo[i].source.x = 0;	bruxo[i].source.y = 0;	bruxo[i].source.w = 23;	bruxo[i].source.h = 34;
		if(i == 0) {
			bruxo[i].rect.x = 20;	bruxo[i].rect.y = -150;	bruxo[i].rect.h = 102;	bruxo[i].rect.w = 82;
		}
		else {
			bruxo[i].rect.x = bruxo[i-1].rect.x + 122;	bruxo[i].rect.y = -150;	bruxo[i].rect.h = 102;	bruxo[i].rect.w = 82;
		}
	}

	heroi.text = loadText(render, "Resources/Heroi.png");
	heroi.source.x = 0;	heroi.source.y = 0;	heroi.source.w = 64;	heroi.source.h = 38;
	heroi.rect.w = 204;	heroi.rect.h = 204;	heroi.rect.x = 415 - (heroi.rect.w)/2;	heroi.rect.y = -48 - (heroi.rect.h)/2;
	heroi.vida = 10000;

	SDL_SetRenderDrawColor(render, 0, 0, 0, 255); //Pinta o render de preto

	while(jogando) {
		frame_start = SDL_GetTicks(); //Pega o frame inicial

		if(game_state == OPENING) { //Inicializa o jogo
			SDL_Delay(1000);
			fadeIn(win, render,"Resources/Sdl-logo.png");
			SDL_Delay(1000);
			fadeOut(win, render,"Resources/Sdl-logo.png");
			game_state = MENU;
		}

		else if(game_state == MENU) { //Logica do menu
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			if(inicio == 0) {
				Backgroundtext = loadText(render, "Resources/Background.png");
				menutext = loadText(render, "Resources/HumanSlayer.png");
				moedatext = loadText(render, "Resources/Moeda.png");
				fadeInMenu(win, render, "Resources/Background.png");
			}
			if(mododejogo == 1) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) { 
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_DOWN:
							state++;
							moedarect.y += 50;
							if(state > QUIT) {
								state = NEW_GAME;
								moedarect.y = 270;
							}
							break;

							case SDL_SCANCODE_UP:
							state--;
							moedarect.y -= 50;
							if(state < NEW_GAME) {
								state = QUIT;
								moedarect.y = 520;
							}
							break;

							case SDL_SCANCODE_RETURN:
							switch(state) {
								case NEW_GAME:
								inicio = 0;
								aperta = 1;
								game_state = HISTORIA;
								break;

								case LOAD_GAME:
								pa = fopen("Save.txt", "r");
								fscanf(pa, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", &game_mode, &player.dest.x, &player.dest.y, &player.source.x, &player.source.y, &player.direction, &player.vida, &player.mana, &player.pontos, &player.elmoceq, &player.peitoralceq, &player.botaceq, &player.elmofeq, &player.peitoralfeq, &player.botafeq, &player.pocaov, &player.pocaom, &player.espadaeq, &player.espada, &player.capacetec, &player.peitoralc, &player.botasc, &player.capacetef, &player.peitoralf, &player.botasf, &player.magiaeq, &mapafore.x, &mapafore.y, &castelodemonrect.x, &castelodemonrect.y, &part1.x, &part1.y, &guardas[0].rect.x, &guardas[0].rect.y, &guardas[1].rect.x, &guardas[1].rect.y, &guardas[2].rect.x, &guardas[2].rect.y, &guardas[3].rect.x, &guardas[3].rect.y, &guardadrect.x, &guardadrect.y, &oraculo.rect.x, &oraculo.rect.y, &spawn_fantasma, &bruxo[0].rect.x, &bruxo[0].rect.y, &bruxo[1].rect.x, &bruxo[1].rect.y, &bruxo[2].rect.x, &bruxo[2].rect.y, &bruxo[3].rect.x, &bruxo[3].rect.y, &bruxo[4].rect.x, &bruxo[4].rect.y, &mododejogo);
								fclose(pa);
								inicio = 0;
								aperta = 1;
								game_state = JOGO;
								break;

								case OPCOES:
								game_state = OPTIONS;
								inicio = 0;
								aperta = 1;
								break;

								case SCORE:
								inicio = 0;
								game_state = RANKING;
								aperta = 1;
								break;

								case CREDITOS:
								game_state = CREDITS;
								inicio = 0;
								aperta = 1;
								break;

								case QUIT:
								jogando = 0;
								inicio = 0;
								break;
							}
						}
						break;

						case SDL_KEYUP:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_DOWN:
							state = state;
							break;

							case SDL_SCANCODE_UP:
							state = state;
							break;
						}
						break;

					}
				}
		}
		else if(mododejogo == 2) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_S:
							state++;
							moedarect.y += 50;
							if(state > QUIT) {
								state = NEW_GAME;
								moedarect.y = 270;
							}
							break;

							case SDL_SCANCODE_W:
							state--;
							moedarect.y -= 50;
							if(state < NEW_GAME) {
								state = QUIT;
								moedarect.y = 520;
							}
							break;

							case SDL_SCANCODE_RETURN:
							switch(state) {
								case NEW_GAME:
								inicio = 0;
								aperta = 1;
								game_state = HISTORIA;
								break;

								case LOAD_GAME:
								pa = fopen("Save.txt", "r");
								fscanf(pa, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", &game_mode, &player.dest.x, &player.dest.y, &player.source.x, &player.source.y, &player.direction, &player.vida, &player.mana, &player.pontos, &player.elmoceq, &player.peitoralceq, &player.botaceq, &player.elmofeq, &player.peitoralfeq, &player.botafeq, &player.pocaov, &player.pocaom, &player.espadaeq, &player.espada, &player.capacetec, &player.peitoralc, &player.botasc, &player.capacetef, &player.peitoralf, &player.botasf, &player.magiaeq, &mapafore.x, &mapafore.y, &castelodemonrect.x, &castelodemonrect.y, &part1.x, &part1.y, &guardas[0].rect.x, &guardas[0].rect.y, &guardas[1].rect.x, &guardas[1].rect.y, &guardas[2].rect.x, &guardas[2].rect.y, &guardas[3].rect.x, &guardas[3].rect.y, &guardadrect.x, &guardadrect.y, &oraculo.rect.x, &oraculo.rect.y, &spawn_fantasma, &bruxo[0].rect.x, &bruxo[0].rect.y, &bruxo[1].rect.x, &bruxo[1].rect.y, &bruxo[2].rect.x, &bruxo[2].rect.y, &bruxo[3].rect.x, &bruxo[3].rect.y, &bruxo[4].rect.x, &bruxo[4].rect.y, &mododejogo);
								fclose(pa);
								inicio = 0;
								aperta = 1;
								game_state = JOGO;
								break;

								case OPCOES:
								game_state = OPTIONS;
								inicio = 0;
								aperta = 1;
								break;

								case SCORE:
								inicio = 0;
								game_state = RANKING;
								aperta = 1;
								break;

								case CREDITOS:
								game_state = CREDITS;
								inicio = 0;
								aperta = 1;
								break;

								case QUIT:
								jogando = 0;
								inicio = 0;
								break;
							}
						}
						break;

						case SDL_KEYUP:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_S:
							state = state;
							break;

							case SDL_SCANCODE_W:
							state = state;
							break;
						}
						break;

					}
				}
		}
			SDL_RenderClear(render);
			SDL_RenderCopy(render, Backgroundtext, NULL, NULL);
			SDL_RenderCopy(render, menutext, NULL, NULL);
			SDL_RenderCopy(render, moedatext, NULL, &moedarect);
			SDL_RenderPresent(render);
			if(aperta == 0) {
				inicio = 1; //Impede o fade de acontecer de novo e permite mexer no fade dos outros modos
			}
		}

		else if(game_state == HISTORIA) {
			if(inicio == 0) {
				fadeInHistoria(win, render, "Resources/Historia.png", historiarect);
				SDL_Delay(2000);
			}

			cont++;
			if(cont >= 2){
				historiarect.y -= 1;
				cont = 0;
			}

			SDL_RenderClear(render);
			SDL_RenderCopy(render, historiatext, NULL, &historiarect);
			SDL_RenderPresent(render);

			if(historiarect.y + historiarect.h < 0) {
				game_state = JOGO;
				inicio = 0;
			}
			inicio = 1;
		}

		else if(game_state == OPTIONS) {
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			moedaVoltar.y = voltar.y + 10;	moedaVoltar.h = moedarect.h;	moedaVoltar.x = voltar.x - 40;	moedaVoltar.w = moedarect.w;

			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						if(dojogo == 0) {
							game_state = MENU;
							aperta = 0;
						}
						else if(dojogo == 1) {
							game_state = PAUSE;
							dojogo = 0;
						}
						break;
						
						case SDL_SCANCODE_RIGHT:
						seta.rect.x = 550;
						mododejogo = 2;
						break;

						case SDL_SCANCODE_A:
						seta.rect.x = 200;
						mododejogo = 1;
						break;
					}
					break;
				}
			}
			SDL_RenderClear(render);
			printTexto(render, vermelho, font, titulorect, "Options");
			SDL_RenderCopy(render, modo, NULL, NULL);
			SDL_RenderCopy(render, seta.text, NULL, &seta.rect);
			SDL_RenderCopy(render, moedatext, NULL, &moedaVoltar);
			printTexto(render, vermelho, font, voltar, "Voltar");
			SDL_RenderPresent(render);
			inicio = 1;
		}

		else if(game_state == CREDITS) {
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			moedaVoltar.y = voltar.y + 10;	moedaVoltar.h = moedarect.h;	moedaVoltar.x = voltar.x - 40;	moedaVoltar.w = moedarect.w;

			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						game_state = MENU;
						aperta = 0;
						break;
					}
					break;
				}
			}


			SDL_RenderClear(render);
			printTexto(render, vermelho, font, titulorect, "Credits");
			printTexto(render, branco, font, telameio, "Guilherme Montenegro");
			SDL_RenderCopy(render, moedatext, NULL, &moedaVoltar);
			printTexto(render, vermelho, font, voltar, "Voltar");
			SDL_RenderPresent(render);
			inicio = 1;
		}

		else if(game_state == RANKING) {
			sortRanking(ranking, pa);

			pontuacaoranking1[3] = (char)(ranking[0].pontos%10) + 48; pontuacaoranking1[2] = (char)((ranking[0].pontos/10)%10) + 48;	pontuacaoranking1[1] = (char)((ranking[0].pontos/100)%10) + 48;	pontuacaoranking1[0] = (char)((ranking[0].pontos/1000)%10) + 48;	pontuacaoranking1[4] = '\0';
			pontuacaoranking2[3] = (char)(ranking[1].pontos%10) + 48; pontuacaoranking2[2] = (char)((ranking[1].pontos/10)%10) + 48;	pontuacaoranking2[1] = (char)((ranking[1].pontos/100)%10) + 48;	pontuacaoranking2[0] = (char)((ranking[1].pontos/1000)%10) + 48;	pontuacaoranking2[4] = '\0';
			pontuacaoranking3[3] = (char)(ranking[2].pontos%10) + 48; pontuacaoranking3[2] = (char)((ranking[2].pontos/10)%10) + 48;	pontuacaoranking3[1] = (char)((ranking[2].pontos/100)%10) + 48;	pontuacaoranking3[0] = (char)((ranking[2].pontos/1000)%10) + 48;	pontuacaoranking3[4] = '\0';

			rankingsrect[0].x = 570;	rankingsrect[0].y = 195;	rankingsrect[0].w = 200;	rankingsrect[0].h = 50;	
			rankingsrect[1].x = 570;	rankingsrect[1].y = 245;	rankingsrect[1].w = 200;	rankingsrect[1].h = 50;	
			rankingsrect[2].x = 570;	rankingsrect[2].y = 295;	rankingsrect[2].w = 200;	rankingsrect[2].h = 50;	

			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			moedaVoltar.y = voltar.y + 10;	moedaVoltar.h = moedarect.h;	moedaVoltar.x = voltar.x - 40;	moedaVoltar.w = moedarect.w;
				
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						game_state = MENU;
						aperta = 0;
						break;
					}
					break;
				}
			}

			SDL_RenderClear(render);
			SDL_RenderCopy(render, moedatext, NULL, &moedaVoltar);
			printTexto(render, vermelho, font, titulorect, "Ranking");
			printTexto(render, vermelho, font, numeros[0], "I");
			printTexto(render, vermelho, font, ranking[0].dest, ranking[0].nome);
			printTexto(render, vermelho, font, rankingsrect[0], pontuacaoranking1);
			printTexto(render, vermelho, font, numeros[1], "II");
			printTexto(render, vermelho, font, ranking[1].dest, ranking[1].nome);
			printTexto(render, vermelho, font, rankingsrect[1], pontuacaoranking2);
			printTexto(render, vermelho, font, numeros[2], "III");
			printTexto(render, vermelho, font, ranking[2].dest, ranking[2].nome);
			printTexto(render, vermelho, font, rankingsrect[2], pontuacaoranking3);
			printTexto(render, vermelho, font, voltar, "Voltar");
			SDL_RenderPresent(render);
			
			inicio = 1;
		}

		else if(game_state == JOGO) {
			SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
			if(inicio == 0) {
				cont = 0;
				contdano = 0;
				contsoco = 0;
			}

			pontuacao[3] = (char)(player.pontos%10) + 48; pontuacao[2] = (char)((player.pontos/10)%10) + 48;	pontuacao[1] = (char)((player.pontos/100)%10) + 48;	pontuacao[0] = (char)((player.pontos/1000)%10) + 48;	pontuacao[4] = '\0';
			pontrect.x = 400;	pontrect.y = 0;	pontrect.w = 100;	pontrect.h = 50;

			fantasma.source.x = 0;	fantasma.source.y = 0;	fantasma.source.h = 25;	fantasma.source.w = 16;

			if(player.armadura <= 0) player.armadura = 0;	if(player.mana <= 0) player.mana = 0;

			if(lanca_magia == 1) {
				player.mana -= 20;
				switch(magiadirection) {
					case 1:
					magiasource.x = 138; magiasource.w = 14;
					magiarect.h = 150;	magiarect.w = 50;
					magiarect.x = player.dest.x;	magiarect.y = player.dest.y + player.dest.h;
					break;

					case 2:
					magiasource.x = 51;	magiasource.w = 30;
					magiarect.h = 100;	magiarect.w = 150;
					magiarect.x = player.dest.x - magiarect.w;	magiarect.y = player.dest.y;
					break;

					case 3:
					magiasource.x = 102;	magiasource.w = 14;
					magiarect.h = 150;	magiarect.w = 50;
					magiarect.x = player.dest.x;	magiarect.y = player.dest.y - magiarect.h;
					break;

					case 4:
					magiasource.x = 0;	magiasource.w = 30;
					magiarect.h = 100;	magiarect.w = 150;
					magiarect.x = player.dest.x + player.dest.w;	magiarect.y = player.dest.y;
					break;
				}
				lanca_magia = 2;
			}

			else if(lanca_magia == 2) {
				switch(magiadirection) {
					case 1:
					magiarect.y += 5;
					if(magiarect.y >= ALTURA) lanca_magia = 0;
					break;

					case 2:
					magiarect.x -= 5;
					if(magiarect.x + magiarect.w <= 0) lanca_magia = 0;
					break;

					case 3:
					magiarect.y -= 5;
					if(magiarect.y + magiarect.h <= 0) lanca_magia = 0;
					break;

					case 4:
					magiarect.x += 5;
					if(magiarect.x >= LARGURA) lanca_magia = 0;
					break;
				}
			}

			if(player.vida == 100) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 0;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 0;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 100 && player.vida >= 90) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 0;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 25;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 90 && player.vida >= 80) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 0;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 80 && player.vida >= 70) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 25;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 70 && player.vida >= 60) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 60 && player.vida >= 50) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 25;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 50 && player.vida >= 40) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 40 && player.vida >= 30) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 25;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 30 && player.vida >= 20) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 50;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 20 && player.vida >= 10) {
				coracaosource[0].x = 25;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 50;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida <= 0) {
				coracaosource[0].x = 50;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 50;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			if(player.mana == 200) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 0;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 0;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 200 && player.mana >= 180) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 0;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 10;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 180 && player.mana >= 160) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 0;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 160 && player.mana >= 140) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 10;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 140 && player.mana >= 120) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 120 && player.mana >= 100) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 10;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 100 && player.mana >= 80) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 80 && player.mana >= 60) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 10;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 60 && player.mana >= 40) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 20;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 40 && player.mana >= 20) {
				manasource[0].x = 10;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 20;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana == 0) {
				manasource[0].x = 20;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 20;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			coracaorect[0].x = 0;	coracaorect[0].y = 0;	coracaorect[0].w = 50;	coracaorect[0].h = 50;
			coracaorect[1].x = 50;	coracaorect[1].y = 0;	coracaorect[1].w = 50;	coracaorect[1].h = 50;
			coracaorect[2].x = 100;	coracaorect[2].y = 0;	coracaorect[2].w = 50;	coracaorect[2].h = 50;
			coracaorect[3].x = 150;	coracaorect[3].y = 0;	coracaorect[3].w = 50;	coracaorect[3].h = 50;
 			coracaorect[4].x = 200;	coracaorect[4].y = 0;	coracaorect[4].w = 50;	coracaorect[4].h = 50;

 			manarect[0].x = 550;	manarect[0].y = 0;	manarect[0].w = 50;	manarect[0].h = 50; 
 			manarect[1].x = 600;	manarect[1].y = 0;	manarect[1].w = 50;	manarect[1].h = 50; 
 			manarect[2].x = 650;	manarect[2].y = 0;	manarect[2].w = 50;	manarect[2].h = 50; 
 			manarect[3].x = 700;	manarect[3].y = 0;	manarect[3].w = 50;	manarect[3].h = 50; 
 			manarect[4].x = 750;	manarect[4].y = 0;	manarect[4].w = 50;	manarect[4].h = 50; 

 			if(mododejogo == 1) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_RETURN:
							case SDL_SCANCODE_P:
							case SDL_SCANCODE_ESCAPE:
							game_state = PAUSE;
							inicio = 0;
							break;

							case SDL_SCANCODE_Z:
							if(player.espadaeq == 0 || player.peitoralceq == 0) {
								player.source.x = 66;
								player.soco = 1;
							}
							break;

							case SDL_SCANCODE_X:
							if(lanca_magia == 0) {
								if(player.magiaeq == 1 && player.mana >= 20) {
									player.magia = 1;
									lanca_magia = 1;
									magiadirection = player.direction;
								}
							}
							break;

							case SDL_SCANCODE_DOWN:
							player.down = 1;
							if(player.elmoceq = 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 0;
							break;

							case SDL_SCANCODE_UP:
							player.up = 1;
							if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 68;
							break;

							case SDL_SCANCODE_LEFT:
							player.left = 1;
							if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 34;
							break;

							case SDL_SCANCODE_RIGHT:
							if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 102;
							player.right = 1;
							break;

							case SDL_SCANCODE_C:
							player.acao = 1;
							break;
						}
						break;

						case SDL_KEYUP:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_DOWN:
							player.down = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = 0;
							}
							break;

							case SDL_SCANCODE_UP:
							player.up = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = player.source.h*2;
							}
							break;

							case SDL_SCANCODE_LEFT:
							player.left = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = player.source.h;
							}
							break;

							case SDL_SCANCODE_RIGHT:
							player.right = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = player.source.h*3;
							}
							break;	

							case SDL_SCANCODE_Z:
							if(player.peitoralceq == 0 || player.espadaeq == 0) {
								player.source.x = 0;
								player.soco = 0;
							}
							break;

							case SDL_SCANCODE_X:
							player.magia = 0;
							break;

							case SDL_SCANCODE_C:
							player.acao = 0;
							break;
						}
						break;
					}
				}
			}

			else if(mododejogo == 2) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_RETURN:
							case SDL_SCANCODE_P:
							case SDL_SCANCODE_ESCAPE:
							game_state = PAUSE;
							inicio = 0;
							break;

							case SDL_SCANCODE_Z:
							if(player.espadaeq == 0 || player.peitoralceq == 0) {
								player.source.x = 66;
								player.soco = 1;
							}
							break;

							case SDL_SCANCODE_X:
							if(lanca_magia == 0) {
								if(player.magiaeq == 1 && player.mana >= 20) {
									player.magia = 1;
									lanca_magia = 1;
									magiadirection = player.direction;
								}
							}
							break;

							case SDL_SCANCODE_S:
							player.down = 1;
							if(player.elmoceq = 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 0;
							break;

							case SDL_SCANCODE_W:
							player.up = 1;
							if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 68;
							break;

							case SDL_SCANCODE_A:
							player.left = 1;
							if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 34;
							break;

							case SDL_SCANCODE_D:
							if(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1) player.source.y = 102;
							player.right = 1;
							break;

							case SDL_SCANCODE_C:
							player.acao = 1;
							break;
						}
						break;

						case SDL_KEYUP:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_S:
							player.down = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = 0;
							}
							break;

							case SDL_SCANCODE_W:
							player.up = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = player.source.h*2;
							}
							break;

							case SDL_SCANCODE_A:
							player.left = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = player.source.h;
							}
							break;

							case SDL_SCANCODE_D:
							player.right = 0;
							if(player.peitoralceq == 0) {
								player.source.x = 0;
								player.source.y = player.source.h*3;
							}
							break;	

							case SDL_SCANCODE_Z:
							if(player.peitoralceq == 0 || player.espadaeq == 0) {
								player.source.x = 0;
								player.soco = 0;
							}
							break;

							case SDL_SCANCODE_X:
							player.magia = 0;
							break;

							case SDL_SCANCODE_C:
							player.acao = 0;
							break;
						}
						break;
					}
				}
			}

			player.speed_y = player.speed_x = 0;
			cont++;
			contsoco++;
			contdano++;

			//Movimentacao e animacao do Player
			if(player.left && !player.right && !player.up && !player.down) { 
				player.speed_x = -SPEED;
				player.direction = 2;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}

			else if(!player.left && player.right && !player.up && !player.down) {
				player.speed_x = SPEED;
				player.direction = 4;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}
			
			else if(player.up && !player.down && !player.left && !player.right) {
				player.speed_y = -SPEED;
				player.direction = 3;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}
			
			else if(!player.up && player.down && !player.left && !player.right) {
				player.speed_y = SPEED;
				player.direction = 1;
				if(player.peitoralceq == 0) {
					if(cont > 12) {
						animation(&player.source, player.direction);
						cont = 0;
					}
				}
			}

			if(game_mode == 0) {
				castelodemonrect.y -= player.speed_y;
				reidemonrect.y -= player.speed_y;

				if(castelodemonrect.y >= -380) {
					castelodemonrect.y += player.speed_y;
					reidemonrect.y += player.speed_y;
				}

				if(castelodemonrect.y >= -390) {
					if(player.acao == 1) {
						if(cont > 25) {
							game_state = FALA;
							fala = 1;
							inicio = 0;
							cont = 0;
						}
					}
				}

				else if(castelodemonrect.y < -1325) {
					game_mode = 1;
				}

				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderPresent(render);

			}

			else if(game_mode == 1) {
				mapafore.x -= player.speed_x;
				mapafore.y -= player.speed_y;
				mapaback.x -= (player.speed_x);
				mapaback.y -= (player.speed_y);
				magiarect.y -= player.speed_y;
				magiarect.x -= player.speed_x;
				if(mapafore.x <= -3210 && mapafore.x >= -3330 && mapafore.y > -650) {
					game_mode = 0;
				}
				if(mapafore.x >= -96 && mapafore.x <= 63 && mapafore.y >= -555) {
					game_mode = 2;
					part1.y = -345;
					fala = 16;
					game_state = FALA;
				}
				else if(mapafore.x <= -3330 && mapafore.y > -660 && mapafore.x >= -4600) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -4044 && mapafore.x >= -4425 && mapafore.y <= -760 && mapafore.y >= -970) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				else if(mapafore.x >= -3210 && mapafore.y > -660 && mapafore.x <= -2900) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -4584 && mapafore.y <= -661) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -3936 && mapafore.y <= -1360) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -3633 && mapafore.y <= -1399) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x >= -3000 && mapafore.y >= -850 && mapafore.x <= -2724){
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -2100 && mapafore.x >= -2676 && mapafore.y >= -886){
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.y <= -1126 && mapafore.x >= -2100 && mapafore.x <= -1850) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.y >= -967 && mapafore.x >= -2100 && mapafore.x <= -1850) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -1026 && mapafore.x >= -1881 && (mapafore.y >= -1000 || mapafore.y <= -1084)) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				} 
				if(mapafore.x <= -2739 && mapafore.x >= 3621 && mapafore.y <= 1369) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x >= -3567 && mapafore.x <= -2880 && mapafore.y <= -1048) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -2103 && mapafore.x >= -2865 && mapafore.y <= -1204) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -3560 && mapafore.x >= -3666 && mapafore.y <= -1372) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x >= 384 || mapafore.y <= -1645) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.x <= -975 && mapafore.x >= -996 && mapafore.y <= -1987) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.x <= -975 && mapafore.x >= -990 && mapafore.y >= -1009) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.y >= -559 && mapafore.x >= -1000 && mapafore.x <= -102) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}
				if(mapafore.y >= -559 && mapafore.x >= 75 && mapafore.x <= 390) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.x <= -4203 && mapafore.x >= -4251 && mapafore.y >= -988) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.y <= -1135 && mapafore.y >= -1555 && mapafore.x >= -1023 && mapafore.x <= -315) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.y >= -1027 && mapafore.x <= -84 && mapafore.x >= -900 && mapafore.y <= -697) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.x <= 381 && mapafore.x >= 78 && mapafore.y >= -1033 && mapafore.y <= -718) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.x <= -3147 && mapafore.y >= -690 && mapafore.x >= -3201) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.x <= 135 && mapafore.x >= 96 && mapafore.y >= -613) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.x >= -156 && mapafore.x <= -114 && mapafore.y >= -604) {
					mapafore.x += player.speed_x;
					mapafore.y += player.speed_y;
					mapaback.x += (player.speed_x);
					mapaback.y += (player.speed_y);
					guardas[0].rect.x += player.speed_x;
					guardas[0].rect.y += player.speed_y;
					guardas[1].rect.x += player.speed_x;
					guardas[1].rect.y += player.speed_y;
					guardas[2].rect.x += player.speed_x;
					guardas[2].rect.y += player.speed_y;
					guardas[3].rect.x += player.speed_x;
					guardas[3].rect.y += player.speed_y;
					oraculo.rect.x += player.speed_x;
					oraculo.rect.y += player.speed_y;
					guardadrect.x += player.speed_x;
					guardadrect.y += player.speed_y;
					fantasma.rect.x += player.speed_x;
					fantasma.rect.y += player.speed_y;
				}

				if(mapafore.x >= -2442 && mapafore.x <= -2160 && mapafore.y >= -907) {
					if(player.soco == 1 && spawn_fantasma == 0) {
						spawn_fantasma = 1;
						fantasma.rect.x = player.dest.x;	fantasma.rect.y = player.dest.y - fantasma.rect.h;
					}
				}

				if(mapafore.x >= -1134 && mapafore.x <= -1071) {
					if(!(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1)) {
						player.left = 0;
						player.direction = 4;
						fala = 6;
						game_state = FALA;
						inicio = 0;
						cont = 0;
						falatext = loadText(render, "Resources/GuardasFala.png");
					}
				}

				if(mapafore.x <= -4203 && mapafore.x >= -4251 && mapafore.y >= -1000) {
					if(cont > 25) {
						if(player.acao == 1) {
							inicio = 0;
							cont = 0;
							fala = 3;
							game_state = FALA;
							falatext = loadText(render, "Resources/OraculoFala1.png");
						}
					}
				}

				if(mapafore.x <= -3147 && mapafore.y >= -700 && mapafore.x >= -3201) {
					if(cont > 25) {
						if(player.acao == 1) {
							inicio = 0;
							cont = 0;
							fala = 7;
							game_state = FALA;
							falatext = loadText(render, "Resources/GuardadFala1.png");
						}
					}
				}

				if(mapafore.x <= 94 && mapafore.x >= -113 && mapafore.y >= -583) {
					if(!(player.elmoceq == 1 && player.peitoralceq == 1 && player.botaceq == 1)) {
						player.up = 0;
						fala = 13;
						game_state = FALA;
						inicio = 0;
						cont = 0;
						proxfala = 0;
						falatext = loadText(render, "Resources/GuardascasteloFala.png");
					}
				}

				if(mapafore.y >= -620 && mapafore.x >= -153 && mapafore.x <= -108) {
					if(cont > 25) {
						if(player.acao == 1) {
							fala = 14;
							game_state = FALA;
							inicio = 0;
							cont = 0;
							proxfala = 0;
							falatext = loadText(render, "Resources/Guardacasteloamigo.png");
						}
					}
				}

				guardas[0].rect.x -= player.speed_x;
				guardas[0].rect.y -= player.speed_y;
				guardas[1].rect.x -= player.speed_x;
				guardas[1].rect.y -= player.speed_y;
				guardas[2].rect.x -= player.speed_x;
				guardas[2].rect.y -= player.speed_y;
				guardas[3].rect.x -= player.speed_x;
				guardas[3].rect.y -= player.speed_y;
				fantasma.rect.x -= player.speed_x;
				fantasma.rect.y -= player.speed_y;

				oraculo.rect.x -= player.speed_x;
				oraculo.rect.y -= player.speed_y;
				guardadrect.x -= player.speed_x;
				guardadrect.y -= player.speed_y;

				if(spawn_fantasma == 1 && cont > 30) {
					fantasma.rect.x = player.dest.x;	fantasma.rect.y = player.dest.y - fantasma.rect.h;
					spawn_fantasma = 2;
					cont = 0;
				}

				if(spawn_fantasma == 2) {
					colisaoMagia(&magiarect, magiadirection, &fantasma, &lanca_magia);
					if(moveInimigo(player, &fantasma) == 1 && contdano >= 60) {
						contdano = 0;
						player.vida -= fantasma.dano - (fantasma.dano*(player.armadura/100));
					}
				}

				if(fantasma.vida <= 0 && spawn_fantasma == 2) {
					spawn_fantasma = 0;
					fantasma.rect.y += 2000;	
					player.pontos += fantasma.pontos;
				}

				castelodemonrect.y = -1310;

				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderPresent(render);
			}

			else if(game_mode == 2) {
				part1.x -= player.speed_x;
				part1.y -= player.speed_y;

				if(part1.y > 330 && part1.x <= 95 && part1.x >= -304) {
					game_mode = 3;
				}

				for(i = 0; i < 7; i++) {
					bruxo[i].rect.x -= player.speed_x;
					bruxo[i].rect.y -= player.speed_y;
					bruxo[i].bala.rect.x -= player.speed_x;

					if(contdano >= 30 && contdano < 60) {
						bruxo[0].atira = 1;
						bruxo[2].atira = 1;
						bruxo[4].atira = 1;
						bruxo[6].atira = 1;
					}

					else if(contdano >= 60) {
						bruxo[1].atira = 1;
						bruxo[3].atira = 1;
						bruxo[5].atira = 1;
						contdano = 0;
					}

					if(bruxo[i].atira == 0) {
						if(bruxo[i].vida > 0) {
							bruxo[i].bala.rect.x = (bruxo[i].rect.x + (bruxo[i].rect.w/2));	bruxo[i].bala.rect.y = bruxo[i].rect.y + bruxo[i].rect.h;
							bruxo[i].bala.rect.w = 40;	bruxo[i].bala.rect.h = 40;
						}
						else {
							bruxo[i].bala.rect.x = 5000;
						}
					}
					else if(bruxo[i].atira == 1) {
						bruxo[i].bala.rect.y += bruxo[i].bala.speed_y;
					}

					if(bruxo[i].bala.rect.y >= ALTURA) {
						bruxo[i].atira = 0;
					}

					colisaoTiro(bruxo[i].bala, &player, &bruxo[i].atira);
					if(contsoco >= 30 && player.soco == 1) {
						contsoco = 0;
						atacarInimigo(player, &bruxo[i]);
					}

					colisaoMagia(&magiarect, magiadirection, &bruxo[i], &lanca_magia);
				}

				if(part1.x <= 20 && part1.x >= -217 && part1.y <= -405) {
					game_mode = 1;
				}

				if(part1.x >= 29 && part1.y <= -408) {
					part1.x += player.speed_x;
					part1.y += player.speed_y;
					for(i = 0; i < 7; i++) {
						bruxo[i].rect.x += player.speed_x;
						bruxo[i].rect.y += player.speed_y;
					}
				}

				if(part1.x >= 374) {
					part1.x += player.speed_x;
					part1.y += player.speed_y;
					for(i = 0; i < 7; i++) {
						bruxo[i].rect.x += player.speed_x;
						bruxo[i].rect.y += player.speed_y;
					}
				}

				if(part1.x <= -220 && part1.y <= -408) {
					part1.x += player.speed_x;
					part1.y += player.speed_y;
					for(i = 0; i < 7; i++) {
						bruxo[i].rect.x += player.speed_x;
						bruxo[i].rect.y += player.speed_y;
					}
				}

				if(part1.x <= -571) {
					part1.x += player.speed_x;
					part1.y += player.speed_y;
					for(i = 0; i < 7; i++) {
						bruxo[i].rect.x += player.speed_x;
						bruxo[i].rect.y += player.speed_y;
					}
				}

				if(part1.x <= -340 && part1.y >= 327) {
					part1.x += player.speed_x;
					part1.y += player.speed_y;
					for(i = 0; i < 7; i++) {
						bruxo[i].rect.x += player.speed_x;
						bruxo[i].rect.y += player.speed_y;
					}
				}

				if(part1.x >= 134 && part1.y >= 327) {
					part1.x += player.speed_x;
					part1.y += player.speed_y;
					for(i = 0; i < 7; i++) {
						bruxo[i].rect.x += player.speed_x;
						bruxo[i].rect.y += player.speed_y;
					}
				}

				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelo_part1, NULL, &part1);
				for(i = 0; i < 7; i++) {
					if(bruxo[i].vida > 0) {
						SDL_RenderCopy(render, bruxo[i].text, &bruxo[i].source, &bruxo[i].rect);
						if(bruxo[i].atira == 1) {
							SDL_RenderCopy(render, bruxo[i].bala.text, NULL, &bruxo[i].bala.rect);
						}
					}
					else {
						bruxo[i].bala.rect.x = 5000;
					}
				}
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderPresent(render);

			}

			else if(game_mode == 3) {
				if(falaheroi == 0) {
					fala = 17;
					inicio = 0;
					cont = 0;
					proxfala = 0;
					falatext = loadText(render, "Resources/Heroifala1.png");
					player.up = 0;
					game_state = FALA;
					falaheroi = 1;
				}
				heroi.rect.x -= player.speed_x;
				heroi.rect.y -= player.speed_y;
				part2rect.x -= player.speed_x;
				part2rect.y -= player.speed_y;

				if(part2rect.y <= -448) {
					heroi.rect.x += player.speed_x;
					heroi.rect.y += player.speed_y;
					part2rect.x += player.speed_x;
					part2rect.y += player.speed_y;
				}

				if(part2rect.y >= 329) {
					heroi.rect.x += player.speed_x;
					heroi.rect.y += player.speed_y;
					part2rect.x += player.speed_x;
					part2rect.y += player.speed_y;
				}

				if(part2rect.x >= 389) {
					heroi.rect.x += player.speed_x;
					heroi.rect.y += player.speed_y;
					part2rect.x += player.speed_x;
					part2rect.y += player.speed_y;
				}

				if(part2rect.x <= -595) {
					heroi.rect.x += player.speed_x;
					heroi.rect.y += player.speed_y;
					part2rect.x += player.speed_x;
					part2rect.y += player.speed_y;
				}

				printf("%d\n", heroi.vida);

				colisaoMagia(&magiarect, magiadirection, &heroi, &lanca_magia);

				if(heroi.vida <= 0) {
					game_state = FALA;
					proxfala = 0;
					inicio = 0;
					cont = 0;
					fala = 18;
					falatext = loadText(render, "Resources/Vitoriafala.png");
					player.pontos += 1000;
				}

				SDL_RenderClear(render);
				SDL_RenderCopy(render, part2text, NULL, &part2rect);
				SDL_RenderCopy(render, heroi.text, &heroi.source, &heroi.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderPresent(render);
			}

			if(cont >= 60) cont = 0;
			if(contdano > 120) contdano = 0;
			if(contsoco > 60) contsoco = 0;

			if(player.vida <= 0) {
				game_state = DERROTA;
			}

			inicio = 1;
		}

		else if(game_state == PAUSE) {
			aperta = 1;
			if(mododejogo == 1) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_DOWN:
							indicadorrect.y += 70;
							pause_state++;
							if(pause_state > PAUSE_QUIT) {
								pause_state = PAUSE_RESUME;
								indicadorrect.y = 55;
							}
							break;
							
							case SDL_SCANCODE_UP:
							indicadorrect.y -= 70;
							pause_state--;
							if(pause_state < PAUSE_RESUME) {
								pause_state = PAUSE_QUIT;
								indicadorrect.y = 475; 
							}
							break;

							case SDL_SCANCODE_P:
							case SDL_SCANCODE_ESCAPE:
							game_state = JOGO;
							break;

							case SDL_SCANCODE_RETURN:
							switch(pause_state) {
								case PAUSE_RESUME:
								game_state = JOGO;
								break;

								case PAUSE_STATUS:
								dano = alocaChar(3);	def = alocaChar(3);
								inicio = 0;
								aperta = 0;
								game_state = STATUS;
								break;

								case PAUSE_INVENTORY:
								potionh = alocaChar(2);	potionm = alocaChar(2);	sword = alocaChar(2); helmetl = alocaChar(2);	helmeti = alocaChar(2);	chestl = alocaChar(2);	chesti = alocaChar(2);	bootsl = alocaChar(2); bootsi = alocaChar(2);
								inicio = 0;
								inventcontrol = 1;
								selecao_inv.x = 450;
								selecao_inv.y = 20;
								game_state = INVENTORY;
								break;

								case PAUSE_SAVE:
								pa = fopen("Save.txt", "w");
								fprintf(pa, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d", game_mode, player.dest.x, player.dest.y, player.source.x, player.source.y, player.direction, player.vida, player.mana, player.pontos, player.elmoceq, player.peitoralceq, player.botaceq, player.elmofeq, player.peitoralfeq, player.botafeq, player.pocaov, player.pocaom, player.espadaeq, player.espada, player.capacetec, player.peitoralc, player.botasc, player.capacetef, player.peitoralf, player.botasf, player.magiaeq, mapafore.x, mapafore.y, castelodemonrect.x, castelodemonrect.y, part1.x, part1.y, guardas[0].rect.x, guardas[0].rect.y, guardas[1].rect.x, guardas[1].rect.y, guardas[2].rect.x, guardas[2].rect.y, guardas[3].rect.x, guardas[3].rect.y, guardadrect.x, guardadrect.y, oraculo.rect.x, oraculo.rect.y, spawn_fantasma, bruxo[0].rect.x, bruxo[0].rect.y, bruxo[1].rect.x, bruxo[1].rect.y, bruxo[2].rect.x, bruxo[2].rect.y, bruxo[3].rect.x, bruxo[3].rect.y, bruxo[4].rect.x, bruxo[4].rect.y, mododejogo);
								fclose(pa);
								SDL_RenderCopy(render, savetext, NULL, NULL);
								SDL_RenderPresent(render);
								SDL_Delay(1000);
								break;

								case PAUSE_LOAD:
								pa = fopen("Save.txt", "r");
								fscanf(pa, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", &game_mode, &player.dest.x, &player.dest.y, &player.source.x, &player.source.y, &player.direction, &player.vida, &player.mana, &player.pontos, &player.elmoceq, &player.peitoralceq, &player.botaceq, &player.elmofeq, &player.peitoralfeq, &player.botafeq, &player.pocaov, &player.pocaom, &player.espadaeq, &player.espada, &player.capacetec, &player.peitoralc, &player.botasc, &player.capacetef, &player.peitoralf, &player.botasf, &player.magiaeq, &mapafore.x, &mapafore.y, &castelodemonrect.x, &castelodemonrect.y, &part1.x, &part1.y, &guardas[0].rect.x, &guardas[0].rect.y, &guardas[1].rect.x, &guardas[1].rect.y, &guardas[2].rect.x, &guardas[2].rect.y, &guardas[3].rect.x, &guardas[3].rect.y, &guardadrect.x, &guardadrect.y, &oraculo.rect.x, &oraculo.rect.y, &spawn_fantasma, &bruxo[0].rect.x, &bruxo[0].rect.y, &bruxo[1].rect.x, &bruxo[1].rect.y, &bruxo[2].rect.x, &bruxo[2].rect.y, &bruxo[3].rect.x, &bruxo[3].rect.y, &bruxo[4].rect.x, &bruxo[4].rect.y, &mododejogo);
								fclose(pa);
								SDL_RenderCopy(render, loadtext, NULL, NULL);
								SDL_RenderPresent(render);
								SDL_Delay(1000);
								break;

								case PAUSE_OPTIONS:
								break;

								case PAUSE_QUIT:
								game_state = MENU;
								aperta = 0;
								inicio = 0;
								break;
							}
							break;
						}	
						break;
					}
				}
				if(aperta == 1) {
					inicio = 1;
				}
			}

			else if(mododejogo == 2) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_S:
							indicadorrect.y += 70;
							pause_state++;
							if(pause_state > PAUSE_QUIT) {
								pause_state = PAUSE_RESUME;
								indicadorrect.y = 55;
							}
							break;
							
							case SDL_SCANCODE_W:
							indicadorrect.y -= 70;
							pause_state--;
							if(pause_state < PAUSE_RESUME) {
								pause_state = PAUSE_QUIT;
								indicadorrect.y = 475; 
							}
							break;

							case SDL_SCANCODE_P:
							case SDL_SCANCODE_ESCAPE:
							game_state = JOGO;
							break;

							case SDL_SCANCODE_RETURN:
							switch(pause_state) {
								case PAUSE_RESUME:
								game_state = JOGO;
								break;

								case PAUSE_STATUS:
								dano = alocaChar(3);	def = alocaChar(3);
								inicio = 0;
								aperta = 0;
								game_state = STATUS;
								break;

								case PAUSE_INVENTORY:
								potionh = alocaChar(2);	potionm = alocaChar(2);	sword = alocaChar(2); helmetl = alocaChar(2);	helmeti = alocaChar(2);	chestl = alocaChar(2);	chesti = alocaChar(2);	bootsl = alocaChar(2); bootsi = alocaChar(2);
								inicio = 0;
								inventcontrol = 1;
								selecao_inv.x = 450;
								selecao_inv.y = 20;
								game_state = INVENTORY;
								break;

								case PAUSE_SAVE:
								pa = fopen("Save.txt", "w");
								fprintf(pa, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d", game_mode, player.dest.x, player.dest.y, player.source.x, player.source.y, player.direction, player.vida, player.mana, player.pontos, player.elmoceq, player.peitoralceq, player.botaceq, player.elmofeq, player.peitoralfeq, player.botafeq, player.pocaov, player.pocaom, player.espadaeq, player.espada, player.capacetec, player.peitoralc, player.botasc, player.capacetef, player.peitoralf, player.botasf, player.magiaeq, mapafore.x, mapafore.y, castelodemonrect.x, castelodemonrect.y, part1.x, part1.y, guardas[0].rect.x, guardas[0].rect.y, guardas[1].rect.x, guardas[1].rect.y, guardas[2].rect.x, guardas[2].rect.y, guardas[3].rect.x, guardas[3].rect.y, guardadrect.x, guardadrect.y, oraculo.rect.x, oraculo.rect.y, spawn_fantasma, bruxo[0].rect.x, bruxo[0].rect.y, bruxo[1].rect.x, bruxo[1].rect.y, bruxo[2].rect.x, bruxo[2].rect.y, bruxo[3].rect.x, bruxo[3].rect.y, bruxo[4].rect.x, bruxo[4].rect.y, mododejogo);
								fclose(pa);
								SDL_RenderCopy(render, savetext, NULL, NULL);
								SDL_RenderPresent(render);
								SDL_Delay(1000);
								break;

								case PAUSE_LOAD:
								pa = fopen("Save.txt", "r");
								fscanf(pa, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", &game_mode, &player.dest.x, &player.dest.y, &player.source.x, &player.source.y, &player.direction, &player.vida, &player.mana, &player.pontos, &player.elmoceq, &player.peitoralceq, &player.botaceq, &player.elmofeq, &player.peitoralfeq, &player.botafeq, &player.pocaov, &player.pocaom, &player.espadaeq, &player.espada, &player.capacetec, &player.peitoralc, &player.botasc, &player.capacetef, &player.peitoralf, &player.botasf, &player.magiaeq, &mapafore.x, &mapafore.y, &castelodemonrect.x, &castelodemonrect.y, &part1.x, &part1.y, &guardas[0].rect.x, &guardas[0].rect.y, &guardas[1].rect.x, &guardas[1].rect.y, &guardas[2].rect.x, &guardas[2].rect.y, &guardas[3].rect.x, &guardas[3].rect.y, &guardadrect.x, &guardadrect.y, &oraculo.rect.x, &oraculo.rect.y, &spawn_fantasma, &bruxo[0].rect.x, &bruxo[0].rect.y, &bruxo[1].rect.x, &bruxo[1].rect.y, &bruxo[2].rect.x, &bruxo[2].rect.y, &bruxo[3].rect.x, &bruxo[3].rect.y, &bruxo[4].rect.x, &bruxo[4].rect.y, &mododejogo);
								fclose(pa);
								SDL_RenderCopy(render, loadtext, NULL, NULL);
								SDL_RenderPresent(render);
								SDL_Delay(1000);
								break;

								case PAUSE_OPTIONS:
								game_state = OPTIONS;
								dojogo = 1;
								break;

								case PAUSE_QUIT:
								game_state = MENU;
								aperta = 0;
								inicio = 0;
								break;
							}
							break;
						}	
						break;
					}
				}
				if(aperta == 1) {
					inicio = 1;
				}
			}

			SDL_RenderClear(render);
			switch(player.direction) {
				case 1:
				player.source.y = 0;
				break;

				case 2:
				player.source.y = player.source.h;
				break;
				
				case 3:
				player.source.y = player.source.h*2;
				break;		

				case 4:
				player.source.y = player.source.h*3;
				break;
			} 

			if(game_mode == 0) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, pausetext, NULL, &pauserect);
				SDL_RenderCopy(render, indicadortext, NULL, &indicadorrect);
				SDL_RenderPresent(render);
			}

			else if(game_mode == 1) {
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, pausetext, NULL, &pauserect);
				SDL_RenderCopy(render, indicadortext, NULL, &indicadorrect);
				SDL_RenderPresent(render);
			}

			else if(game_mode == 2) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelo_part1, NULL, &part1);
				for(i = 0; i < 7; i++) {
					if(bruxo[i].vida > 0) {
						SDL_RenderCopy(render, bruxo[i].text, &bruxo[i].source, &bruxo[i].rect);
						if(bruxo[i].atira == 1) {
							SDL_RenderCopy(render, bruxo[i].bala.text, NULL, &bruxo[i].bala.rect);
						}
					}
					else {
						bruxo[i].bala.rect.x = 5000;
					}
				}
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, pausetext, NULL, &pauserect);
				SDL_RenderCopy(render, indicadortext, NULL, &indicadorrect);
				SDL_RenderPresent(render);
			}

			else if(game_mode == 3) {
				SDL_RenderCopy(render, part2text, NULL, &part2rect);
				SDL_RenderCopy(render, heroi.text, &heroi.source, &heroi.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, pausetext, NULL, &pauserect);
				SDL_RenderCopy(render, indicadortext, NULL, &indicadorrect);
				SDL_RenderPresent(render);
			}

			if(player.vida == 0) {
				game_state = DERROTA;
			}
		}

		else if(game_state == STATUS) {
			status_capacete.x = 370;	status_capacete.y = 10;		status_capacete.w = 150;	status_capacete.h = 150;
			status_peitoral.x = 390;	status_peitoral.y = 200;	status_peitoral.w = 150;	status_peitoral.h = 150;
			status_bota.x = 390;	status_bota.y = 400;	status_bota.w = 150;	status_bota.h = 150;
			status_espada.x = 50;	status_espada.y = 200;	status_espada.w = 150;	status_espada.h = 150;

			def[0] = (char)(player.armadura/10) + 48;
			def[1] = (char)(player.armadura%10) + 48;
			def[2] = '\0';
			dano[0] = (char)(player.dano/10) + 48;
			dano[1] = (char)(player.dano%10) + 48;
			dano[2] = '\0';
			defrect.x = 660;	defrect.y = 100;	defrect.w = 90;	defrect.h = 70;
			danorect.x = 660;	danorect.y = 170;	danorect.w = 90; danorect.h = 70;
			voltar_status.x = 610; voltar_status.y = 530;	voltar_status.w = 30;	voltar_status.h = 30;
			player_status.x = 185; player_status.y = 10;	player_status.w = 240;	player_status.h = 550;
			player_status_source.x = 0;	player_status_source.y = 0;	player_status_source.w = player.source.w;	player_status_source.h = player.source.h;

			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						case SDL_SCANCODE_ESCAPE:
						game_state = PAUSE;
						desalocChar(def);
						desalocChar(dano);
						break;
					}
					break;
				}
			}


			SDL_RenderClear(render);
			SDL_RenderCopy(render, statustext, NULL, NULL);
			SDL_RenderCopy(render, moedatext, NULL, &voltar_status);
			SDL_RenderCopy(render, player.text, &player_status_source, &player_status);
			printTexto(render, marrom, font, defrect, def);
			printTexto(render, marrom, font, danorect, dano);
			if(player.elmoceq == 1) {
				capaceteeq = loadText(render, "Resources/Capuz.png");
				SDL_RenderCopy(render, capaceteeq, NULL, &status_capacete);
			}
			else if(player.elmofeq == 1) {
				capaceteeq = loadText(render, "Resources/Elmodeferro.png");
				SDL_RenderCopy(render, capaceteeq, NULL, &status_capacete);
			}
			if(player.peitoralceq == 1) {
				peitoraleq = loadText(render, "Resources/Tunicadecouro.png");
				SDL_RenderCopy(render, peitoraleq, NULL, &status_peitoral);
			}
			else if(player.peitoralfeq == 1) {
				peitoraleq = loadText(render, "Resources/Peitoraldeferro.png");
				SDL_RenderCopy(render, peitoraleq, NULL, &status_peitoral);
			}
			if(player.botaceq == 1) {
				botaeq = loadText(render, "Resources/Botasdecouro.png");
				SDL_RenderCopy(render, botaeq, NULL, &status_bota);
			}
			else if(player.botafeq == 1) {
				botaeq = loadText(render, "Resources/Botasdeferro.png");
				SDL_RenderCopy(render, botaeq, NULL, &status_bota);
			}
			if(player.espadaeq == 1) {
				espadaeq = loadText(render, "Resources/EspadadeFogo.png");
				SDL_RenderCopy(render, espadaeq, NULL, &status_espada);
			}
			SDL_RenderPresent(render);
			inicio = 1;
		}

		else if(game_state == INVENTORY) {
			if(player.vida == 100) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 0;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 0;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 100 && player.vida >= 90) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 0;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 25;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 90 && player.vida >= 80) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 0;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 80 && player.vida >= 70) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 25;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 70 && player.vida >= 60) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 0;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 60 && player.vida >= 50) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 25;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 50 && player.vida >= 40) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 0;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 40 && player.vida >= 30) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 25;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 30 && player.vida >= 20) {
				coracaosource[0].x = 0;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 50;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida < 20 && player.vida >= 10) {
				coracaosource[0].x = 25;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 50;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			else if(player.vida <= 0) {
				coracaosource[0].x = 50;	coracaosource[0].y = 0;	coracaosource[0].w = 25;	coracaosource[0].h = 25;
				coracaosource[1].x = 50;	coracaosource[1].y = 0;	coracaosource[1].w = 25;	coracaosource[1].h = 25;
				coracaosource[2].x = 50;	coracaosource[2].y = 0;	coracaosource[2].w = 25;	coracaosource[2].h = 25;
				coracaosource[3].x = 50;	coracaosource[3].y = 0;	coracaosource[3].w = 25;	coracaosource[3].h = 25;
				coracaosource[4].x = 50;	coracaosource[4].y = 0;	coracaosource[4].w = 25;	coracaosource[4].h = 25;
			}

			if(player.mana == 200) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 0;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 0;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 200 && player.mana >= 180) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 0;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 10;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 180 && player.mana >= 160) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 0;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 160 && player.mana >= 140) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 10;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 140 && player.mana >= 120) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 0;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 120 && player.mana >= 100) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 10;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 100 && player.mana >= 80) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 0;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 80 && player.mana >= 60) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 10;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 60 && player.mana >= 40) {
				manasource[0].x = 0;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 20;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana < 40 && player.mana >= 20) {
				manasource[0].x = 10;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 20;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			else if(player.mana == 0) {
				manasource[0].x = 20;	manasource[0].y = 0;	manasource[0].w = 10;	manasource[0].h = 12;
				manasource[1].x = 20;	manasource[1].y = 0;	manasource[1].w = 10;	manasource[1].h = 12;
				manasource[2].x = 20;	manasource[2].y = 0;	manasource[2].w = 10;	manasource[2].h = 12;
				manasource[3].x = 20;	manasource[3].y = 0;	manasource[3].w = 10;	manasource[3].h = 12;
				manasource[4].x = 20;	manasource[4].y = 0;	manasource[4].w = 10;	manasource[4].h = 12;
			}

			potionh[0] = (char)(player.pocaov) + 48; potionh[1] = '\0';	potionh_inv.x = 730;	potionh_inv.y = 20;	potionh_inv.h = 50;	potionh_inv.w = 50;
			potionm[0] = (char)(player.pocaom) + 48; potionm[1] = '\0'; potionm_inv.x = 730;	potionm_inv.y = 70;	potionm_inv.h = 50;	potionm_inv.w = 50;
			sword[0] = (char)(player.espada) + 48; sword[1] = '\0';	sword_inv.x = 730;	sword_inv.y = 270;	sword_inv.h = 50;	sword_inv.w = 50;
			helmetl[0] = (char)(player.capacetec) + 48; helmetl[1] = '\0';	helmetl_inv.x = 730;	helmetl_inv.y = 120;	helmetl_inv.h = 50;	helmetl_inv.w = 50;
			helmeti[0] = (char)(player.capacetef) + 48; helmeti[1] = '\0';	helmeti_inv.x = 730;	helmeti_inv.y = 310;	helmeti_inv.h = 50;	helmeti_inv.w = 50;
			chestl[0] = (char)(player.peitoralc) + 48; chestl[1] = '\0';	chestl_inv.x = 730;	chestl_inv.y = 170;	chestl_inv.h = 50;	chestl_inv.w = 50;
			chesti[0] = (char)(player.peitoralf) + 48; chesti[1] = '\0';	chesti_inv.x = 730;	chesti_inv.y = 350;	chesti_inv.h = 50;	chesti_inv.w = 50;
			bootsl[0] = (char)(player.botasc) + 48; bootsl[1] = '\0';	bootsl_inv.x = 730;	bootsl_inv.y = 220;	bootsl_inv.h = 50;	bootsl_inv.w = 50;
			bootsi[0] = (char)(player.botasf) + 48; bootsi[1] = '\0';	bootsi_inv.x = 730;	bootsi_inv.y = 400;	bootsi_inv.h = 50;	bootsi_inv.w = 50;
			
			if(inicio == 0) {
				cont = 0;
			} 

			if(mododejogo == 1) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_DOWN:
							selecao_inv.y += 50;
							inventcontrol++;
							if(inventcontrol > 10) {
								inventcontrol = 1;
								selecao_inv.x = 450;
								selecao_inv.y = 20;
							} 
							else if(inventcontrol == 10) {
								selecao_inv.x = 640;
								selecao_inv.y = 540;
							}
							break;

							case SDL_SCANCODE_RETURN:
							if(inventcontrol == 10) {
								desalocChar(potionh);	desalocChar(potionm);	desalocChar(sword);	desalocChar(helmetl);	desalocChar(helmeti);	desalocChar(chestl);	desalocChar(chesti);	desalocChar(bootsl);	desalocChar(bootsi);
								game_state = PAUSE;
							}
							else {
								selecao = 1;
							}
							break;

							case SDL_SCANCODE_UP:
							selecao_inv.y -= 50;
							inventcontrol--;
							if(inventcontrol < 1) {
								inventcontrol = 10;
								selecao_inv.x = 640;
								selecao_inv.y = 540;
							}
							else if(inventcontrol == 9) {
								selecao_inv.x = 450;
								selecao_inv.y = 420;
							}
							break;
						}
					}
				}
			}

			else if(mododejogo == 1) {
				while(SDL_PollEvent(&e)) {
					switch(e.type) {
						case SDL_QUIT:
						jogando = 0;
						break;

						case SDL_KEYDOWN:
						switch(e.key.keysym.scancode) {
							case SDL_SCANCODE_S:
							selecao_inv.y += 50;
							inventcontrol++;
							if(inventcontrol > 10) {
								inventcontrol = 1;
								selecao_inv.x = 450;
								selecao_inv.y = 20;
							} 
							else if(inventcontrol == 10) {
								selecao_inv.x = 640;
								selecao_inv.y = 540;
							}
							break;

							case SDL_SCANCODE_RETURN:
							if(inventcontrol == 10) {
								desalocChar(potionh);	desalocChar(potionm);	desalocChar(sword);	desalocChar(helmetl);	desalocChar(helmeti);	desalocChar(chestl);	desalocChar(chesti);	desalocChar(bootsl);	desalocChar(bootsi);
								game_state = PAUSE;
							}
							else {
								selecao = 1;
							}
							break;

							case SDL_SCANCODE_W:
							selecao_inv.y -= 50;
							inventcontrol--;
							if(inventcontrol < 1) {
								inventcontrol = 10;
								selecao_inv.x = 640;
								selecao_inv.y = 540;
							}
							else if(inventcontrol == 9) {
								selecao_inv.x = 450;
								selecao_inv.y = 420;
							}
							break;
						}
					}
				}
			}

			cont++;

			SDL_RenderClear(render);
			SDL_RenderCopy(render, inventory, NULL, NULL);
			SDL_RenderCopy(render, moedatext, NULL, &selecao_inv);
			printTexto(render, marrom, font, potionh_inv, potionh);
			printTexto(render, marrom, font, potionm_inv, potionm);
			printTexto(render, marrom, font, sword_inv, sword);
			printTexto(render, marrom, font, helmetl_inv, helmetl);
			printTexto(render, marrom, font, helmeti_inv, helmeti);
			printTexto(render, marrom, font, chestl_inv, chestl);
			printTexto(render, marrom, font, chesti_inv, chesti);
			printTexto(render, marrom, font, bootsl_inv, bootsl);
			printTexto(render, marrom, font, bootsi_inv, bootsi);
			switch(inventcontrol) {
				case 1:
				itemmostra = loadText(render, "Resources/Pocaocura.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(cont > 25) {
					if(selecao == 1) {
						selecao = 0;
						cont = 0;
						if(player.pocaov > 0) {
							if(player.vida < 100) {
								player.vida += 20;
								if(player.vida > 100) player.vida = 100;
								player.pocaov--;
							}
						}
					}
				} 
				break;

				case 2:
				itemmostra = loadText(render, "Resources/Pocaomana.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(cont > 25) {
						selecao = 0;
						cont = 0;
						if(player.pocaom > 0) {
							if(player.mana < 200) {
								player.mana += 40;
								if(player.mana > 200) player.mana = 200;
								player.pocaom--;
							}
						}
					}
				}
				break;

				case 3:
				itemmostra = loadText(render, "Resources/Capuz.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(cont > 25) {
						selecao = 0;
						cont = 0;
						if(player.capacetec == 1) {
							if(player.elmoceq == 0) {
								if(player.elmofeq == 0) {
									player.elmoceq = 1;
									player.capacetec--;
									player.armadura += 5;
									player.source.x = 90;
								}
								else {
									player.elmofeq = 0;
									player.capacetef = 1;
									player.elmoceq = 1;
									player.capacetec--;
									player.armadura += 5;
									player.source.x = 90;
								}
							}
						}
						else if(player.capacetec == 0 && player.elmoceq == 1) {
							player.elmoceq = 0;
							player.capacetec = 1;
							player.armadura -= 5;
							player.source.x = 0;
						}
					}
				}
				break;

				case 4:
				itemmostra = loadText(render, "Resources/Tunicadecouro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(cont > 25) {
						selecao = 0;
						cont = 0;
						if(player.peitoralc == 1) {
							if(player.peitoralceq == 0) {
								if(player.peitoralfeq == 0) {
									player.peitoralceq = 1;
									player.peitoralc--;
									player.armadura += 10;
									player.source.x = 90;
								}
								else {
									player.peitoralfeq = 0;
									player.peitoralf = 1;
									player.peitoralceq = 1;
									player.peitoralc--;
									player.armadura += 10;
									player.source.x = 90;
								}
							}
						}
						else if(player.peitoralc == 0 && player.peitoralceq == 1) {
							player.peitoralceq = 0;
							player.peitoralc = 1;
							player.armadura -= 10;
						}
					}
				}
				break;

				case 5:
				itemmostra = loadText(render, "Resources/Botasdecouro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(cont > 25) {
						selecao = 0;
						cont = 0;
						if(player.botasc == 1) {
							if(player.botaceq == 0) {
								if(player.botafeq == 0) {
									player.botaceq = 1;
									player.botasc--;
									player.armadura += 5;
								}
								else {
									player.botafeq == 0;
									player.botasf = 1;
									player.botaceq = 1;
									player.botasc--;
									player.armadura += 5;
								}
							}
						}
						else if(player.botasc == 0 && player.botaceq == 1){
							player.botaceq = 0;
							player.botasc = 1;
							player.armadura -= 5;
						}
					}
				}
				break;

				case 6:
				itemmostra = loadText(render, "Resources/EspadadeFogo.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(cont > 25) {
						selecao = 0;
						cont = 0;
						if(player.espada == 1) {
							if(player.espadaeq == 0) {
								player.espadaeq = 1;
								player.espada--;
								player.dano += 40;
							}
						}
						else if(player.espada == 0 && player.espadaeq == 1) {
							player.espadaeq = 0;
							player.espada = 1;
							player.dano -= 40;
						}
					}
				}
				break;

				case 7:
				itemmostra = loadText(render, "Resources/Elmodeferro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(cont > 25) {
						selecao = 0;
						cont = 0;
						if(player.capacetef == 1) {
							if(player.elmofeq == 0) {
								if(player.elmoceq == 0) {
									player.elmofeq = 1;
									player.capacetef--;
									player.armadura += 10;
								}
								else {
									player.elmoceq = 0;
									player.capacetec = 1;
									player.elmofeq = 1;
									player.capacetef--;
									player.armadura += 10;
									player.source.x = 0;
								}
							}
						}
						else if(player.capacetef == 0 && player.elmofeq == 1) {
							player.elmofeq = 0;
							player.capacetef = 1;
							player.armadura -= 10;
						}
					}
				}
				break;

				case 8:
				itemmostra = loadText(render, "Resources/Peitoraldeferro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(selecao == 1) {
					if(cont > 25) {
						selecao = 0;
						cont = 0;
						if(player.peitoralf == 1) {
							if(player.peitoralfeq == 0) {
								if(player.peitoralceq == 0) {
									player.peitoralfeq = 1;
									player.peitoralf--;
									player.armadura += 20;
								}
								else {
									player.peitoralceq = 0;
									player.peitoralc = 1;
									player.peitoralfeq = 1;
									player.peitoralf--;
									player.armadura += 20;							
								}
							}
						}
						else if(player.peitoralf == 0 && player.peitoralfeq == 1) {
							player.peitoralfeq = 0;
							player.peitoralf = 1;
							player.armadura -= 20;
						}
					}
				}
				break;

				case 9:
				itemmostra = loadText(render, "Resources/Botasdeferro.png");
				SDL_RenderCopy(render, itemmostra, NULL, &iteminvent);
				if(cont > 25) {
					if(selecao == 1) {
						selecao = 0;
						cont = 0;
						if(player.botasf == 1) {
							if(player.botafeq == 0) {
								if(player.botaceq == 0) {
									player.botafeq = 1;
									player.botasf--;
									player.armadura += 10;
								}
								else {
									player.botaceq = 0;
									player.botasc = 1;
									player.botafeq = 1;
									player.botasf--;
									player.armadura += 10;
								}
							}
						}
						else if(player.botasf == 0 && player.botafeq == 1) {
							player.botafeq = 0;
							player.botasf = 1;
							player.armadura -= 10;
						}
					}
				}
				break;
			}

			inicio = 1;
			SDL_RenderPresent(render);
		}

		else if(game_state == FALA) {

			if(inicio == 0) {
				cont = 0;
			}
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_C:
						proxfala = 1;
						break;
					}
					break;

					case SDL_KEYUP:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_C:
						proxfala = 0;
						break;
					}
					break;
				}
			}
			cont++;

			if(fala == 1) {
				falatext = loadText(render, "Resources/ReiFala1.png");
				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);
				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						falatext = loadText(render, "Resources/ReiFala2.png");
						fala = 2;
					}
				}
			}

			else if(fala == 2) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, castelodemontext, NULL, &castelodemonrect);
				SDL_RenderCopy(render, rei_demontext, NULL, &reidemonrect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
					}
				}
			}

			else if(fala == 3) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						falatext = loadText(render, "Resources/OraculoFala2.png");
						fala = 4;
					}
				}
			}

			else if(fala == 4) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						falatext = loadText(render, "Resources/OraculoFala3.png");
						fala = 5;
					}
				}
			}

			else if(fala == 5) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						if(player.magiaeq = 0) {
							fala = 8;
							falatext = loadText(render, "Resources/Magia++.png");
						}
						else {
							fala = 0;
							game_state = JOGO;
							player.magiaeq = 1;
						}
					}
				}
			}

			else if(fala == 6) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
						mapafore.x -= 50;
						mapaback.x -= 50;
						guardas[0].rect.x -= 50;
						guardas[1].rect.x -= 50;
						guardas[2].rect.x -= 50;
						guardas[3].rect.x -= 50;
						oraculo.rect.x -= 50;
						guardadrect.x -= 50;
					}
				}
			}
			else if(fala == 7) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						if(player.capacetec == 0 && player.elmoceq == 0) {
							player.capacetec = 1;	player.peitoralc = 1;	player.botasc = 1; player.pocaov = 5;	player.pocaom = 5; player.espada = 1;
							fala = 9;
							falatext = loadText(render, "Resources/Tunica++.png");
						}
						else {
							fala = 0;
							game_state = JOGO;
						}
					}
				}
			}

			else if(fala == 8) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
					}
				}
			}

			else if(fala == 9) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 10;
						falatext = loadText(render, "Resources/Cura++.png");
					}
				}
			}

			else if(fala == 10) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 11;
						falatext = loadText(render, "Resources/Mana++.png");
					}
				}
			}

			else if(fala == 11) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 12;
						falatext = loadText(render, "Resources/Espada++.png");
					}
				}
			}

			else if(fala == 12) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						game_state = JOGO;
					}
				}
			}

			else if(fala == 13) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						inicio = 0;
						cont = 0;
						game_state = DERROTA;
					}
				}
			}

			if(fala == 14) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						if(player.elmofeq == 0 && player.capacetef == 0) {
							player.capacetef = 1; player.peitoralf = 1; player.botasf = 1;
							fala = 15;
							inicio = 0;
							cont = 0;
							falatext = loadText(render, "Resources/Setferro++.png");
						}
						else {
							fala = 0;
							game_state = JOGO;
						}
					}
				}
			}

			if(fala == 15) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, mapaforetext, NULL, &mapafore);
				SDL_RenderCopy(render, guardad, NULL, &guardadrect);
				SDL_RenderCopy(render, guardas[0].text, &guardas[0].source, &guardas[0].rect);
				SDL_RenderCopy(render, guardas[2].text, &guardas[2].source, &guardas[2].rect);
				SDL_RenderCopy(render, guardas[3].text, &guardas[3].source, &guardas[3].rect);
				SDL_RenderCopy(render, oraculo.text, NULL, &oraculo.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				if(spawn_fantasma == 2) {
					SDL_RenderCopy(render, fantasma.text, &fantasma.source, &fantasma.rect);
				}
				SDL_RenderCopy(render, guardas[1].text, &guardas[1].source, &guardas[1].rect);
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						inicio = 0;
						cont = 0;
						game_state = JOGO;
					}
				}
			}

			if(fala == 16) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, part2text, NULL, &part2rect);
				SDL_RenderCopy(render, heroi.text, &heroi.source, &heroi.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 17;
						inicio = 0;
						cont = 0;
						falatext = loadText(render, "Resources/Heroifala2.png");
					}
				}
			}

			if(fala == 17) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, part2text, NULL, &part2rect);
				SDL_RenderCopy(render, heroi.text, &heroi.source, &heroi.rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						inicio = 0;
						cont = 0;
						game_state = JOGO;
					}
				}
			}

			if(fala == 18) {
				SDL_RenderClear(render);
				SDL_RenderCopy(render, part2text, NULL, &part2rect);
				SDL_RenderCopy(render, player.text, &player.source, &player.dest);
				if(lanca_magia != 0) {
					SDL_RenderCopy(render, magiatext, &magiasource, &magiarect);
				}
				SDL_RenderCopy(render, coracao, &coracaosource[0], &coracaorect[0]);
				SDL_RenderCopy(render, coracao, &coracaosource[1], &coracaorect[1]);
				SDL_RenderCopy(render, coracao, &coracaosource[2], &coracaorect[2]);
				SDL_RenderCopy(render, coracao, &coracaosource[3], &coracaorect[3]);
				SDL_RenderCopy(render, coracao, &coracaosource[4], &coracaorect[4]);
				SDL_RenderCopy(render, manahud, &manasource[0], &manarect[0]);
				SDL_RenderCopy(render, manahud, &manasource[1], &manarect[1]);
				SDL_RenderCopy(render, manahud, &manasource[2], &manarect[2]);
				SDL_RenderCopy(render, manahud, &manasource[3], &manarect[3]);
				SDL_RenderCopy(render, manahud, &manasource[4], &manarect[4]);
				SDL_RenderCopy(render, falatext, NULL, NULL);
				SDL_RenderPresent(render);

				if(cont > 25) {
					cont = 0;
					if(proxfala == 1) {
						fala = 0;
						inicio = 0;
						cont = 0;
						game_state = VITORIA;
					}
				}
			}


			inicio = 1;
		}		

		else if(game_state == DERROTA) {
			game_state = SCORE_MARK;
			player.nome = alocaChar(11);
			player.pontos += 5*(player.vida);
			for(i = 0; i < 11; i++) {
				player.nome[i] = ' ';
				player.nome[10] = '\0';
			}
			i = 0;
			SDL_RenderClear(render);
			SDL_RenderCopy(render, derrotatext, NULL, NULL);
			SDL_RenderPresent(render);

			SDL_Delay(2000);
		}

		else if(game_state == VITORIA) {
			game_state = SCORE_MARK;
			player.nome = alocaChar(11);
			player.pontos += 5*(player.vida);
			player.pontos += 5000;
			for(i = 0; i < 11; i++) {
				player.nome[i] = ' ';
				player.nome[10] = '\0';
			}
			i = 0;
			SDL_RenderClear(render);
			SDL_RenderCopy(render, vitoriatext, NULL, NULL);
			SDL_RenderPresent(render);

			SDL_Delay(2000);
		}

		else if(game_state == SCORE_MARK) {
			if(i > 10) {
				i--;
			}
			while(SDL_PollEvent(&e)) {
				switch(e.type) {
					case SDL_QUIT:
					jogando = 0;
					break;

					case SDL_KEYDOWN:
					switch(e.key.keysym.scancode) {
						case SDL_SCANCODE_RETURN:
						reescreveArq(ranking, pa, player);
						game_state = MENU;
						break;
						
						case SDL_SCANCODE_A:
						if(i < 10) {
							player.nome[i] = 'A';
						}
						i++;
						break;

						case SDL_SCANCODE_B:
						if(i < 10) {
							player.nome[i] = 'B';
						}
						i++;
						break;

						case SDL_SCANCODE_C:
						if(i < 10) {
							player.nome[i] = 'C';
						}
						i++;
						break;

						case SDL_SCANCODE_D:
						if(i < 10) {
							player.nome[i] = 'D';
						}
						i++;
						break;

						case SDL_SCANCODE_E:
						if(i < 10) {
							player.nome[i] = 'E';
						}
						i++;
						break;

						case SDL_SCANCODE_F:
						if(i < 10) {
							player.nome[i] = 'F';
						}
						i++;
						break;

						case SDL_SCANCODE_G:
						if(i < 10) {
							player.nome[i] = 'G';
						}
						i++;
						break;

						case SDL_SCANCODE_H:
						if(i < 10) {
							player.nome[i] = 'H';
						}
						i++;
						break;
					
						case SDL_SCANCODE_I:
						if(i < 10) {	
							player.nome[i] = 'I';
						}
						i++;
						break;

						case SDL_SCANCODE_J:
						if(i < 10) {	
							player.nome[i] = 'J';
						}
						i++;
						break;

						case SDL_SCANCODE_K:
						if(i < 10) {	
							player.nome[i] = 'K';
						}
						i++;
						break;

						case SDL_SCANCODE_L:
						if(i < 10) {	
							player.nome[i] = 'L';
						}
						i++;
						break;

						case SDL_SCANCODE_M:
						if(i < 10) {	
							player.nome[i] = 'M';
						}
						i++;
						break;

						case SDL_SCANCODE_N:
						if(i < 10) {
							player.nome[i] = 'N';
						}
						i++;
						break;

						case SDL_SCANCODE_O:
						if(i < 10) {	
							player.nome[i] = 'O';
						}
						i++;
						break;

						case SDL_SCANCODE_P:
						if(i < 10) {	
							player.nome[i] = 'P';
						}
						i++;
						break;

						case SDL_SCANCODE_Q:
						if(i < 10) {
							player.nome[i] = 'Q';
						}
						i++;
						break;

						case SDL_SCANCODE_R:
						if(i < 10) {	
							player.nome[i] = 'R';
						}
						i++;
						break;

						case SDL_SCANCODE_S:
						if(i < 10) {	
							player.nome[i] = 'S';
						}
						i++;
						break;

						case SDL_SCANCODE_T:
						if(i < 10) {	
							player.nome[i] = 'T';
						}
						i++;
						break;

						case SDL_SCANCODE_U:
						if(i < 10) {
							player.nome[i] = 'U';
						}
						i++;
						break;

						case SDL_SCANCODE_V:
						if(i < 10) {	
							player.nome[i] = 'V';
						}
						i++;
						break;

						case SDL_SCANCODE_W:
						if(i < 10) {
							player.nome[i] = 'W';
						}
						i++;
						break;

						case SDL_SCANCODE_X:
						if(i < 10) {	
							player.nome[i] = 'X';
						}
						i++;
						break;

						case SDL_SCANCODE_Y:
						if(i < 10) {	
							player.nome[i] = 'Y';
						}
						i++;
						break;

						case SDL_SCANCODE_Z:
						if(i < 10) {
							player.nome[i] = 'Z';
						}
						i++;
						break;

						case SDL_SCANCODE_BACKSPACE:
						if(i > -1) {
							if(i != 10) {	
								player.nome[i-1] = ' ';
							}
							else if(i == 10) {
								player.nome[i-1] = ' ';
							}
							if(i > 0) {
								i--;
							}
						}
						break;

						case SDL_SCANCODE_0:
						if(i < 10) {
							player.nome[i] = '0';
						}
						i++;
						break;

						case SDL_SCANCODE_1:
						if(i < 10) {	
							player.nome[i] = '1';
						}
						i++;
						break;

						case SDL_SCANCODE_2:
						if(i < 10) {	
							player.nome[i] = '2';
						}
						i++;
						break;

						case SDL_SCANCODE_3:
						if(i < 10) {	
							player.nome[i] = '3';
						}
						i++;
						break;

						case SDL_SCANCODE_4:
						if(i < 10) {	
							player.nome[i] = '4';
						}
						i++;
						break;

						case SDL_SCANCODE_5:
						if(i < 10) {
							player.nome[i] = '5';
						}
						i++;
						break;

						case SDL_SCANCODE_6:
						if(i < 10) {	
							player.nome[i] = '6';
						}
						i++;
						break;

						case SDL_SCANCODE_7:
						if(i < 10) {
							player.nome[i] = '7';
						}
						i++;
						break;

						case SDL_SCANCODE_8:
						if(i < 10) {	
							player.nome[i] = '8';
						}
						i++;
						break;

						case SDL_SCANCODE_9:
						if(i < 10) {
							player.nome[i] = '9';
						}
						i++;
						break;
					}
					break;

					case SDL_KEYUP:
					break;
				}
			}

			inicio = 1;

			SDL_RenderClear(render);
			SDL_RenderCopy(render, scoretext, NULL, NULL);
			printTexto(render, branco, font, digite, player.nome);
			SDL_RenderPresent(render);
		}

		//Limita os frames do jogo

		frame_time = SDL_GetTicks() - frame_start; 
		if(frame_delay > frame_time) {
			SDL_Delay(frame_delay - frame_time);
		}

	}
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

char *alocaChar(int tam) {
	char *vet;

	vet = (char*)malloc(sizeof(char)*tam);
	return vet;
}

void desalocChar(char *vet) {
	free(vet);
}

SDL_Texture *loadText(SDL_Renderer *r, char *path) {
	SDL_Surface *temp = IMG_Load(path);
	SDL_Texture *text = SDL_CreateTextureFromSurface(r, temp);
	SDL_FreeSurface(temp);

	return text;
}

void fadeIn(SDL_Window *w, SDL_Renderer *r, char *path) {
	int opacidade = 0;
	SDL_Texture *text;

	while (opacidade < 255) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, NULL);
		SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
		SDL_RenderPresent(r);

		opacidade += 2;

		SDL_RenderClear(r);
	}

}

void fadeInHistoria(SDL_Window *w, SDL_Renderer *r, char *path, SDL_Rect pos) {
	int opacidade = 0;
	SDL_Texture *text;

	while (opacidade < 255) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, &pos);
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_RenderPresent(r);

		opacidade += 2;

		SDL_RenderClear(r);
	}

}

void fadeInMenu(SDL_Window *w, SDL_Renderer *r, char *path) {
	int opacidade = 0;
	SDL_Texture *text;

	while (opacidade < 255) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, NULL);
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_RenderPresent(r);

		opacidade += 1;

		SDL_RenderClear(r);
	}

}

void fadeOut(SDL_Window *w, SDL_Renderer *r, char *path) {
	int opacidade = 255;
	SDL_Texture *text;

	while (opacidade > 0) {

		text = loadText(r, path);
		SDL_SetTextureBlendMode(text, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(text, opacidade);
		SDL_RenderCopy(r, text, NULL, NULL);
		SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
		SDL_RenderPresent(r);

		opacidade -= 2;

		SDL_RenderClear(r);
	}

	SDL_DestroyTexture(text);
}

void printTexto(SDL_Renderer *r, SDL_Color color, TTF_Font *font, SDL_Rect pos, char *frase) {
	SDL_Surface *textSurf = TTF_RenderText_Solid(font, frase, color);
	SDL_Texture *text;
	text = SDL_CreateTextureFromSurface(r, textSurf);
	SDL_FreeSurface(textSurf);

	SDL_RenderCopy(r, text, NULL, &pos);
}

void sortRanking(Player *ranking, FILE *pa) {
	int i, j, temp;
	char *tempc;

	pa = fopen("Ranking.txt", "r");
	for(i = 0; i < 4; i++) {
		ranking[i].nome = alocaChar(11);
		fscanf(pa, "%s", ranking[i].nome);
	}
	fclose(pa);

	pa = fopen("Pontos.txt", "r");
	for(i = 0; i < 4; i++) {
		fscanf(pa, "%d", &ranking[i].pontos);
	}
	fclose(pa);

	for(i = 0; i < 4; i++) {
		for(j = i+1; j < 4; j++){
			if(ranking[j].pontos > ranking[i].pontos) {
				tempc = ranking[j].nome;
				ranking[j].nome = ranking[i].nome;
				ranking[i].nome = tempc;

				temp = ranking[j].pontos;
				ranking[j].pontos = ranking[i].pontos;
				ranking[i].pontos = temp;
				i = 0;
			}
		}
	}
	
	pa = fopen("Ranking.txt", "w");
	for(i = 0; i < 4; i++) {
		fprintf(pa, "%s\n", ranking[i].nome);
	}
	fclose(pa);

	pa = fopen("Pontos.txt", "w");
	for(i = 0; i < 4; i++) {
		fprintf(pa, "%d\n", ranking[i].pontos);
	}
	fclose(pa);
}

void reescreveArq(Player *ranking, FILE *pa, Player p4) {
	int i;
	pa = fopen("Ranking.txt", "r");
	for(i = 0; i < 4; i++) {
		ranking[i].nome = alocaChar(11);
		fscanf(pa, "%s", ranking[i].nome);
	}
	fclose(pa);

	pa = fopen("Pontos.txt", "r");
	for(i = 0; i < 4; i++) {
		fscanf(pa, "%d", &ranking[i].pontos);
	}
	fclose(pa);

	ranking[3].nome = p4.nome;
	ranking[3].pontos = p4.pontos;

	pa = fopen("Ranking.txt", "w");
	for(i = 0; i < 4; i++) {
		fprintf(pa, "%s\n", ranking[i].nome);
	}
	fclose(pa);

	pa = fopen("Pontos.txt", "w");
	for(i = 0; i < 4; i++) {
		fprintf(pa, "%d\n", ranking[i].pontos);
	}
	fclose(pa);
}

void animation(SDL_Rect *source, int direction) {
	switch(direction) {
		case 1:
		source->x += LARGURA_PLAYER/3;
		source->y = 0;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = 0;
		}
		break;
		
		case 2:
		source->x += LARGURA_PLAYER/3;
		source->y = source->h;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = source->h;
		}
		break;

		case 3:
		source->x += LARGURA_PLAYER/3;
		source->y = source->h*2;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = source->h*2;
		}
		break;

		case 4:
		source->x += LARGURA_PLAYER/3;
		source->y = source->h*3;
		if(source->x >= LARGURA_PLAYER) {
			source->x = 0;
			source->y = source->h*3;
		}
		break;
	}
}

int moveInimigo(Player p, Inimigo *i) {
	if(i->rect.x + i->rect.w > p.dest.x) {
		i->rect.x -= i->speed_x;
		if(i->rect.y < p.dest.y + p.dest.h) {
			i->rect.y += i->speed_y;
		}

		if(i->rect.y + i->rect.h > p.dest.y) {
			i->rect.y -= i->speed_y;
		}
	}

	if(i->rect.x < p.dest.x + p.dest.w) {
		i->rect.x += i->speed_x;
		if(i->rect.y < p.dest.y + p.dest.h) {
			i->rect.y += i->speed_y;
		}

		if(i->rect.y + i->rect.h > p.dest.y) {
			i->rect.y -= i->speed_y;
		}
	}

	if(i->rect.x + i->rect.w > p.dest.x && i->rect.x < p.dest.x + p.dest.w)  {
		if(i->rect.y + i->rect.h > p.dest.y && i->rect.y < p.dest.y + p.dest.h) {
			return 1;
		}
	}

}

int atacarInimigo(Player p, Inimigo *i) {
	if(p.direction == 1 && (i->rect.x + i->rect.w > p.dest.x && i->rect.x < p.dest.x + p.dest.w) && p.dest.y <= i->rect.y) {
		if(p.soco == 1) {
			i->vida -= p.dano;
			i->source.x += i->source.w;
		}
	}

	else if(p.direction == 2 && (i->rect.y + i->rect.h > p.dest.y && i->rect.y < p.dest.y + p.dest.h) && p.dest.x <= i->rect.x) {
		if(p.soco == 1) {
			i->vida -= p.dano;
			i->source.x += i->source.w;
		}
	}

	else if(p.direction == 3 && (i->rect.x + i->rect.w > p.dest.x && i->rect.x < p.dest.x + p.dest.w) && p.dest.y >= i->rect.y) {
		if(p.soco == 1) {
			i->vida -= p.dano;
			i->source.x += i->source.w;
		}
	}

	else if(p.direction == 4 && (i->rect.y + i->rect.h > p.dest.y && i->rect.y < p.dest.y + p.dest.h) && p.dest.x >= i->rect.x) {
		if(p.soco == 1) {
			i->vida -= p.dano;
			i->source.x += i->source.w;
		}
	}
}

int colisaoMagia(SDL_Rect *magiarect, int magiadirection, Inimigo *i, int *lanca_magia) {
	if(lanca_magia != 0) {
		if(magiadirection == 1 && (i->rect.x + i->rect.w > magiarect->x && i->rect.x < magiarect->x + magiarect->w) && magiarect->y <= i->rect.y) {
			i->vida -= 75;
			lanca_magia = 0;
		}

		else if(magiadirection == 2 && (i->rect.y + i->rect.h > magiarect->y && i->rect.y < magiarect->y + magiarect->h) && (magiarect->x >= i->rect.x && magiarect->x <= i->rect.x + i->rect.w) || magiarect->x + magiarect->w >= i->rect.x && magiarect->x + magiarect->w <= i->rect.x + i->rect.w) {
			i->vida -= 75;
			lanca_magia = 0;
		}

		else if(magiadirection == 3 && (i->rect.x + i->rect.w > magiarect->x && i->rect.x < magiarect->x + magiarect->w) && magiarect->y >= i->rect.y) {
			i->vida -= 75;
			lanca_magia = 0;

		}

		else if(magiadirection == 4 && (i->rect.y + i->rect.h > magiarect->y && i->rect.y < magiarect->y + magiarect->h) && (magiarect->x + magiarect->w >= i->rect.x && magiarect->x + magiarect->w <= i->rect.x + i->rect.w || magiarect->x >= i->rect.x && magiarect->x <= i->rect.x + i->rect.w)) {
			i->vida -= 75;
			lanca_magia = 0;

		}
	}
}

int colisaoTiro(Tiro tiro, Player *p, int *atira) {
	if(tiro.rect.x + tiro.rect.w > p->dest.x && tiro.rect.x < p->dest.x + p->dest.w) {
		if(tiro.rect.y + tiro.rect.h >= p->dest.y && tiro.rect.y <= p->dest.y + p->dest.h) {
			p->vida -= 10 - (10*(p->armadura/100));
			if(p->vida <= 20) {
				p->vida = 0;
			}
			*atira = 0;
		}
	}
}