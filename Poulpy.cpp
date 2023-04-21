// apps/LIFAMI/TPComplex.cpp
#include<Grapic.h>
#include<math.h>

using namespace grapic;

// Constantes de la fenêtre

const int DIMX = 1280;
const int DIMY = 720;

// Une constante qui va adapter la vitesse d'exécution

const float step = 1.f;

// Constante pour définir le nombre maximal d'entités

const int MAX_ENTITES = 15;

// Structure pour manipuler la 2D

struct Vec2
{
    float x,y;
};

// Générateur
// Fonction qui génère et renvoie une Vec2 selon les données fournis (axe X et Y ou angle)

Vec2 gener_vec2(float x, float y)
{
    Vec2 vec2;
    vec2.x = x;
    vec2.y = y;
    return vec2;
}

Vec2 gener_vec2_exponentielle(float module, float angleDegree)
{
    Vec2 vec2;
    float angleRadian = angleDegree*2.0*M_PI/360;
    vec2.x = module*cos(angleRadian);
    vec2.y = module*sin(angleRadian);
    return vec2;
}

// Opérateurs
// Fonctions pour manipuler les localisations de manière plus pratique

Vec2 operator+(Vec2 vec2A, Vec2 vec2B)
{
    return gener_vec2(vec2A.x+vec2B.x, vec2A.y+vec2B.y);
}

Vec2 operator-(Vec2 vec2A, Vec2 vec2B)
{
    return gener_vec2(vec2A.x-vec2B.x, vec2A.y-vec2B.y);
}

Vec2 operator-(Vec2 vec2)
{
    return gener_vec2(-vec2.x, -vec2.y);
}

Vec2 operator*(float lambda, Vec2 vec2)
{
    return gener_vec2(lambda*vec2.x, lambda*vec2.y);
}

Vec2 operator*(Vec2 vec2A, Vec2 vec2B)
{
    return gener_vec2(vec2A.x*vec2B.x-vec2A.y*vec2B.y, vec2A.x*vec2B.y+vec2A.y*vec2B.x);
}

Vec2 operator+=(Vec2 vec2A, Vec2 vec2B)
{
    return gener_vec2(vec2A.x+vec2B.x, vec2A.y+vec2B.y);
}

Vec2 rotation(Vec2 vec2, float centreX, float centreY, float angleDegree)
{
    Vec2 centre = gener_vec2(centreX, centreY);
    Vec2 rotation = gener_vec2_exponentielle(1, angleDegree);
    return (vec2-centre)*rotation+centre;
}

float distance(Vec2 vec2A, Vec2 vec2B)
{
    return std::sqrt(std::pow(vec2B.x-vec2A.x, 2) + std::pow(vec2B.y - vec2B.y, 2));
}

// Poulpe

struct Poulpe
{
    Vec2 position, vitesse, viseur;
};

// Entités quelconques
// type 0: bulle d'encre
// type 1: crabe
// type 2: nulle

struct Entite
{
    Vec2 position, vitesse;
    int type;
    float vie, lifetime;
    bool enVie;
};

// Fonction pour créer une entité

Entite creer(Vec2 position, Vec2 vitesse, int type, float vie, float lifetime)
{
    Entite entite;
    entite.position = position;
    entite.vitesse = vitesse;
    entite.type = type;
    entite.vie = vie;
    entite.lifetime = lifetime;
    entite.enVie = true;
    return entite;
}

Entite creer(Vec2 position, Vec2 vitesse, int type, float vie, bool enVie, float lifetime)
{
    Entite entite;
    entite.position = position;
    entite.vitesse = vitesse;
    entite.type = type;
    entite.vie = vie;
    entite.enVie = enVie;
    entite.lifetime =lifetime;
    return entite;
}

// Fonction pour nettoyer les entités mortes

void nettoyer(Entite entites[MAX_ENTITES])
{
    int offset = 0;
    for(int i = 0; i<10; i++)
    {
        if(entites[i].vie == 0)
        {
            offset++;

        }
        entites[i].vie = entites[i+offset].vie;
        entites[i].enVie = entites[i+offset].enVie;
    }
    for(int i = offset; i>0; i--)
    {
        entites[10-i].vie = 999;
    }
}

// Fonction qui va tenter de faire apparaitre une entité

void apparition(Entite entites[MAX_ENTITES], Entite entite)
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(entites[i].vie == 999)
        {
            entites[i] = entite;
            i = MAX_ENTITES;
        }
    }
}

// Fonction pour initialiser la liste d'entités

void initialisation(Entite entites[MAX_ENTITES])
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        entites[i] = creer(gener_vec2(0,0), gener_vec2(0,0), 2, 999, false);
    }
}

// Fonction pour tuer une entité

void tuer(Entite entites[MAX_ENTITES], int index)
{
    entites[index] = creer(gener_vec2(0,0), gener_vec2(0,0), 2, 999, false);
}

Entite creer_crabe(Vec2 position, Vec2 vitesse)
{
    Entite entite;
    entite.position = position;
    entite.vitesse = vitesse;
    entite.type = 1;
    entite.vie = 10;
    entite.enVie = true;
    entite.lifetime = INT_MAX;
    return entite;
}

// Fonction pour faire bouger le pouple dans l'espace 2D en fonction des touches appuyées

void deplace(Poulpe &poulpe)
{
    if(isKeyPressed(SDLK_UP))
    {
        poulpe.vitesse.y = step*10;
    }
    if(isKeyPressed(SDLK_DOWN))
    {
        poulpe.vitesse.y = step*-10;
    }
    if(isKeyPressed(SDLK_RIGHT))
    {
        poulpe.vitesse.x = step*10;
    }
    if(isKeyPressed(SDLK_LEFT))
    {
        poulpe.vitesse.x = step*-10;
    }
}

// Actualise la position du Pouple en fonction de sa vitesse

void actualise(Poulpe &poulpe)
{
    poulpe.position = poulpe.position + poulpe.vitesse;
    poulpe.vitesse = 0.95f*poulpe.vitesse;
    float angle = atan2(-poulpe.vitesse.y,-poulpe.vitesse.x)*180/M_PI;
    poulpe.viseur = 100*gener_vec2_exponentielle(1, angle);
}

// Fonction pour calculer la trajectoire des ennemis

void calculTrajectoireEnemis(Poulpe poulpe, Entite &entite)
{
    float X = poulpe.position.x - entite.position.x;
    float Y = poulpe.position.y - entite.position.y;
    Vec2 vitesse = gener_vec2_exponentielle(1, atan2 (Y,X) * 180 / M_PI);
    if (entite.type == 1)
    {
        entite.vitesse = vitesse;
    }

}

void calculDegatsEnemis(Entite &entite, Entite entites[MAX_ENTITES])
{
    for (int i = 0 ; i < MAX_ENTITES; i++)
    {
        float taille = 100-entites[i].lifetime*0.267;
        if (distance(entites[i].position, entite.position) < (100-entites[i].lifetime*0.267+50) && entites[i].type == 0)
        {
            // On veut que le crabe meurt en 10 secondes de contact avec une bulle d'encre, on a donc le rapport entre 60 ips et 10 secondes
            entite.vie = entite.vie - 0.016;
        }
    }
}

// Fonction qui actualise les entités

void actualiseEntites(Poulpe poulpe, Entite entites[MAX_ENTITES])
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(entites[i].lifetime <= 0 || entites[i].vie < 0)
            tuer(entites, i);
        if(entites[i].type == 1 && (entites[i].position.x > DIMX+50 || entites[i].position.y > DIMY+50 || entites[i].position.x < -50 || entites[i].position.y < -50))
        {
            tuer(entites, i);
            apparition(entites, creer_crabe(gener_vec2(500,500), gener_vec2(0,0)));
        }
        entites[i].position = entites[i].position + entites[i].vitesse;
        if(entites[i].enVie)
        {
            entites[i].lifetime--;
            switch(entites[i].type)
            {
                case 0:
                    entites[i].vitesse = 0.95f*entites[i].vitesse;
                    break;
                case 1:
                    calculTrajectoireEnemis(poulpe, entites[i]);
                    calculDegatsEnemis(entites[i], entites);
                    break;
            }
        }
    }
    nettoyer(entites);
}

// Affiche le poulpe à l'écran

void dessine(Poulpe poulpe)
{
    color( 31, 114, 165 );
    circleFill(poulpe.position.x+2, poulpe.position.y-2,30);
    color(255,0,0);
    circleFill(poulpe.position.x, poulpe.position.y, 30);
    color(0,255,0);
    line(poulpe.position.x, poulpe.position.y, poulpe.position.x+poulpe.viseur.x,poulpe.position.y+poulpe.viseur.y);
}

void jet(Poulpe poulpe, Entite entites[MAX_ENTITES])
{
    if(isKeyPressed(SDLK_SPACE))
    {
        apparition(entites, creer(poulpe.position, 0.1*poulpe.viseur, 0, 10, 300));
    }
}

void dessineEntites(Entite entites[MAX_ENTITES])
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(entites[i].enVie)
        {
            switch(entites[i].type)
            {
                case 0:
                    /* On veut qu'une bulle d'encre disparaisse au bout de 5 s.
                       Elle a une durée de vie de 300 (60 ips * 5 s).
                       Je fait donc un rapport entre 255 (opacité maximale) et sa durée de vie pour réduire l'opacité = 0.85.*/
                    color(0,0,0,entites[i].lifetime*0.85);
                    // On veut que la taille de la bulle d'encre augmente en 5 s jusqu'à une taille de 100 avec une taille initiale à 20.
                    circleFill(entites[i].position.x,entites[i].position.y,100-entites[i].lifetime*0.267);
                    break;
                case 1:
                    color(255, 87, 51);
                    circleFill(entites[i].position.x,entites[i].position.y,50);
                    break;
                default:
                    break;
            }
        }
    }
}

void menuPrincipal(Image menu, bool &jouant)
{
    image_draw(menu,0,0);
    int x,y;
    mousePos(x,y);
    if(isMousePressed(SDL_BUTTON_LEFT))
    {
        if(x>417 && x<862)
        {
            if(y>258 && y<376)
            {
                std::cout << "Started !" << std::endl;
                jouant = true;
            } else if(y>124 && y < 232)
            {
                exit(0);
            }
        }
    }
}

void boutonPause(Image bouton, bool &jouant, bool &enPause)
{
    image_draw(bouton, 1280-32, 720-32);
    int x,y;
    mousePos(x,y);
    if(isMousePressed(SDL_BUTTON_LEFT))
    {
        if(x>1280-32 && y >720-32)
        {
            enPause = true;
            jouant = false;
        }
    }
}

void menuPause(Image pause);

int main(int , char**)
{
	winInit("Pouply",DIMX,DIMY);
	backgroundColor( 18, 156, 235 );
	bool jouant = false, enPause = false, run;
	Poulpe poulpe;
	poulpe.position = gener_vec2(500,500);
	poulpe.vitesse = gener_vec2(0,0);

	Entite entites[MAX_ENTITES];
	initialisation(entites);

	apparition(entites, creer_crabe(gener_vec2(0,0), gener_vec2(0,0)));

	Image menu = image("data/Poulpy/menu.png");
    Image pause = image("data/Poulpy/pause.png");

    float time1, time2;

	do{
        winClear();
        if(jouant)
        {
            time1 = elapsedTime();
            time2 = elapsedTime();
            while((time2 - time1)*1000 < 17)
            {
                time2 = elapsedTime();
            }
            deplace(poulpe);
            actualise(poulpe);
            jet(poulpe, entites);
            actualiseEntites(poulpe, entites);
            dessineEntites(entites);
            dessine(poulpe);
            boutonPause(pause, jouant, enPause);
            //effet_bulle(poulpe);
        } else if (enPause) {
            menuPrincipal(menu, jouant);
        }else if (!jouant && !enPause){
            menuPrincipal(menu, jouant);
        }
        run = winDisplay();
	}while(!run);
	winQuit();
	return 0;
}

