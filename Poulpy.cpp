// apps/LIFAMI/TPComplex.cpp
#include<Grapic.h>
#include<math.h>

#define INT_MAX 2147483647

using namespace grapic;

// Constantes de la fenêtre
const int DIMX = 1280;
const int DIMY = 720;

// Une constante qui va adapter la vitesse d'exécution
const float step = 1.f;

// Constante pour définir le nombre maximal d'entités
const int MAX_ENTITES = 30;

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
Vec2 operator/(Vec2 vec2, float lambda)
{
    return gener_vec2(vec2.x/lambda, vec2.y/lambda);
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
    return std::sqrt(std::pow(vec2B.x-vec2A.x, 2) + std::pow(vec2B.y - vec2A.y, 2));
}

/*  Structure de l'entité
    Entités quelconques
    type 0: bulle d'encre
    type 1: crabe
    type 2: nulle, morte
*/
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
// Fonction qui permet de générer une entité nulle
Entite nulle()
{
    Entite entite;
    entite.position = gener_vec2(0,0);
    entite.vitesse = gener_vec2(0,0);
    entite.type = 2;
    entite.vie = 0;
    entite.lifetime = 0;
    entite.enVie = false;
    return entite;
}
// Fonction qui créé un crabe
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

/*
    Fonctions liées aux entités -----------------------------------------------
*/

// Fonction qui va tenter de faire apparaitre une entité
void apparition(Entite entites[MAX_ENTITES], Entite entite)
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(entites[i].type == 2)
        {
            entites[i] = entite;
            i = MAX_ENTITES;
        }
    }
}
// Fonction pour tuer une entité
void tuer(Entite entites[MAX_ENTITES], int index)
{
    entites[index] = nulle();
}
// Fonction pour nettoyer les entités mortes
void nettoyer(Entite entites[MAX_ENTITES])
{
    int offset = 0;
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(entites[i].type == 2)
        {
            offset++;
        }
        entites[i] = entites[i+offset];
    }
    for(int i = offset; i>0; i--)
    {
        entites[MAX_ENTITES-i] = nulle();
    }
}

/*
    Structure du poulpe
*/
struct Poulpe
{
    Vec2 position, vitesse, viseur;
    float vie;
    Image image;
};

/*
    Structure du jeu
*/
struct Jeu
{
    Poulpe poulpe;
    Entite crabes[MAX_ENTITES];
    Entite bulleEncre[MAX_ENTITES];
    int etat, score, nbCrabes, nbBulles, nVague, sous;
    float delaiDegats, delaiTir, finVague;
    bool enAttenteDeVague;
    Image imgCrabe, imgBulle;
};

// Fonction pour initialiser les listes d'entités du jeu

void initialisationEntitesJeu(Jeu &jeu)
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        jeu.crabes[i] = nulle();
        jeu.bulleEncre[i] = nulle();
    }
}

/*
    Poulpe --------------------------------------------------------------------
*/

//                         Actualisation/Fonctionnement

// Fonction qui permet de calculer les dégats causés par les crabes au poulpe
void calculDegatPoulpe(Jeu &jeu)
{
    bool faireDegat = false;
    /* On limite la prise de dégat du poulpe à toutes les 1 secondes de contact avec un ou plusieurs crabes
    La prise de dégats se fait au contact du poulpe avec l'un des crabes
    DelaiDegat est défini dans la fonction main et est actualisé ici */
    if(elapsedTime()-jeu.delaiDegats > 1)
    {
        for(int i = 0; i< MAX_ENTITES; i++)
        {
            // Ici le 80 représente 50+30, respectivement taille de crabe + taille de poulpe
            if (distance(jeu.poulpe.position, jeu.crabes[i].position) < 80 && !faireDegat)
            {
                faireDegat = true;
                break;
            }
        }
        if(faireDegat)
        {
            jeu.poulpe.vie -= 5;
            jeu.delaiDegats = elapsedTime();
        }
    }
}
// Fonction pour faire bouger le pouple dans l'espace 2D en fonction des touches appuyées
void deplace(Jeu &jeu)
{
    if(isKeyPressed(SDLK_UP))
    {
        jeu.poulpe.vitesse.y = step*10;
    }
    if(isKeyPressed(SDLK_DOWN))
    {
        jeu.poulpe.vitesse.y = step*-10;
    }
    if(isKeyPressed(SDLK_RIGHT))
    {
        jeu.poulpe.vitesse.x = step*10;
    }
    if(isKeyPressed(SDLK_LEFT))
    {
        jeu.poulpe.vitesse.x = step*-10;
    }
}
// Fonction qui détecte quand il faut tirer une bulle d'encre
void jet(Jeu &jeu)
{
    if(isKeyPressed(SDLK_SPACE) && elapsedTime()-jeu.delaiTir>0.5)
    {
        apparition(jeu.bulleEncre, creer(jeu.poulpe.position, 0.1*jeu.poulpe.viseur, 0, 10, 300));
        jeu.delaiTir = elapsedTime();
    }
}
// Fonction qui gère les actions du joueur
void actionsPoulpe(Jeu &jeu)
{
    deplace(jeu);
    jet(jeu);
}
// Actualise la position du Pouple en fonction de sa vitesse
void actualisePoulpe(Jeu &jeu)
{
    jeu.poulpe.position = jeu.poulpe.position + jeu.poulpe.vitesse;
    jeu.poulpe.vitesse = 0.95f*jeu.poulpe.vitesse;
    float angle = atan2(-jeu.poulpe.vitesse.y,-jeu.poulpe.vitesse.x)*180/M_PI;
    jeu.poulpe.viseur = 100*gener_vec2_exponentielle(1, angle);
    if(jeu.poulpe.vie <= 0)
    {
        jeu.etat = 3;
    }
}

//                                 Affichage

// Fonction pour dessiner la barre de vie du poulpe
void barreVie(Poulpe poulpe)
{
    color(0,0,0);
    rectangleFill((DIMX)/2-100, DIMY-60, (DIMX)/2+100, DIMY-40);
    color(255,0,0);
    float t = poulpe.vie/50;
    rectangleFill((DIMX)/2-97, DIMY-57, (DIMX)/2-97+(t)*194, DIMY-43);
}
// Fonction pour dessiner le poulpe
void dessinePoulpe(Jeu jeu)
{
    // On change la couleur lorsque le poulpe prend des dégats, pendant 0.2 secondes
    // DelaiDegat cf. calculDegatPoulpe()
    if(elapsedTime()-jeu.delaiDegats<0.2)
        color(255,0,0);
    else
        color(100,0,0);
    //circleFill(poulpe.position.x, poulpe.position.y, 30);
    image_draw(jeu.poulpe.image, jeu.poulpe.position.x-30, jeu.poulpe.position.y-30, 60,60, atan2(jeu.poulpe.vitesse.y,-jeu.poulpe.vitesse.x)*180/M_PI-90);
    print(DIMX+5,DIMY-20, jeu.nVague);
    barreVie(jeu.poulpe);
}

/*
    Crabes ---------------------------------------------------------------------
*/

//                        Actualisation/Fonctionnement

// Fonction pour calculer les collisions entre crabes
void calculCollisionCrabes(Jeu &jeu, int index)
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(distance(jeu.crabes[index].position, jeu.crabes[i].position) < 100 && i != index)
        {
            Vec2 vec;
            do{
                Vec2 tampon = jeu.crabes[index].position-jeu.crabes[i].position;
                vec = gener_vec2_exponentielle(1, atan2(tampon.y, tampon.x)*180/M_PI);
                jeu.crabes[index].position = jeu.crabes[index].position + vec/2.f;
                jeu.crabes[i].position = jeu.crabes[i].position - vec/2.f;
            } while(distance(jeu.crabes[index].position, jeu.crabes[i].position) < 100);
        }
    }
}

// Fonction pour calculer la trajectoire des crabes
void calculTrajectoireCrabes(Jeu &jeu, int index)
{
    // On veut diriger les crabes vers le poulpe
    float X = jeu.poulpe.position.x - jeu.crabes[index].position.x;
    float Y = jeu.poulpe.position.y - jeu.crabes[index].position.y;
    Vec2 vitesse = gener_vec2_exponentielle(1, atan2 (Y,X) * 180 / M_PI);
    jeu.crabes[index].vitesse = vitesse;
    calculCollisionCrabes(jeu, index);
}
// Fonction qui permet de calculer les dégats causé aux crabes par les bulles d'encre
void calculDegatsCrabes(Jeu &jeu, int index)
{
    for (int i = 0 ; i < MAX_ENTITES; i++)
    {
        float taille = 100-jeu.bulleEncre[i].lifetime*0.267;
        if (distance(jeu.bulleEncre[i].position, jeu.crabes[index].position) < (100-jeu.bulleEncre[i].lifetime*0.267+50) && jeu.bulleEncre[i].type == 0 && jeu.bulleEncre[i].lifetime > 150)
        {
            // On veut que le crabe meurt en 10 secondes de contact avec une bulle d'encre, on a donc le rapport entre 60 ips et 10 secondes
            jeu.crabes[index].vie = jeu.crabes[index].vie - 0.016;
        }
    }
}
// Fonction pour actualiser un crabe à l'index fourni
void actualiseCrabe(Jeu &jeu, int index)
{
    if(jeu.crabes[index].vie<0)
    {
        jeu.score++;
        jeu.sous++;
        tuer(jeu.crabes, index);
    }
    jeu.crabes[index].position = jeu.crabes[index].position + jeu.crabes[index].vitesse;
    jeu.crabes[index].lifetime--;
    calculTrajectoireCrabes(jeu, index);
    calculDegatsCrabes(jeu, index);
    jeu.nbCrabes++;
    nettoyer(jeu.crabes);
}
// Fonction pour faire apparaitre une vague de crabe autour de l'espace de jeu
void gener_vague_crabe(Jeu &jeu)
{
    Vec2 position;
    for (int i = 0; i < jeu.nVague+5; i++)
    {
        position = gener_vec2(DIMX/2,DIMY/2)+gener_vec2_exponentielle(frand(300,350), frand(0,360));
        apparition(jeu.crabes, creer_crabe(position, gener_vec2(0,0)));
    }
    jeu.nVague++;
    std::cout << "Generation d'une vague de crabe" << std::endl;
}
// Fonction qui gère les vagues et leur déclenchement
void vague(Jeu &jeu)
{
    if(jeu.enAttenteDeVague)
    {
        if(elapsedTime()-jeu.finVague > 5)
        {
            gener_vague_crabe(jeu);
            jeu.enAttenteDeVague = false;
            jeu.nVague++;
        }
    } else if(jeu.nbCrabes == 0)
    {
        jeu.finVague = elapsedTime();
        jeu.enAttenteDeVague = true;
    }
}


//                                  Affichage

// Fonction pour afficher la barre de vie d'un crabe
void barreVieCrabe(Jeu jeu, int index)
{
    float tVie = jeu.crabes[index].vie/10.0;
    color(0,255,0);
    rectangleFill(jeu.crabes[index].position.x-45, jeu.crabes[index].position.y-45, jeu.crabes[index].position.x-45+tVie*90, jeu.crabes[index].position.y-40);
}
// Fonction pour dessiner les crabes
void dessineCrabes(Jeu jeu)
{
    for (int i = 0; i<MAX_ENTITES; i++)
    {
        if(jeu.crabes[i].type == 1) {
            image_draw(jeu.imgCrabe,jeu.crabes[i].position.x-50,jeu.crabes[i].position.y-50, 100,100);
            barreVieCrabe(jeu, i);
        }
    }
}


/*
    Bulles d'encre -------------------------------------------------------------
*/

//                          Actualisation/Fonctionnement

// Fonction pour actualiser une bulle d'encre à l'index fourni
void actualiseBulleEncre(Jeu &jeu, int index)
{
    if(jeu.bulleEncre[index].lifetime <= 0)
    {
        tuer(jeu.bulleEncre, index);
    }
    jeu.bulleEncre[index].position = jeu.bulleEncre[index].position + jeu.bulleEncre[index].vitesse;
    jeu.bulleEncre[index].lifetime--;
    jeu.bulleEncre[index].vitesse = 0.95f*jeu.bulleEncre[index].vitesse;
}

//                                   Affichage

// Fonction pour dessiner les bulles d'encre
void dessineBulles(Jeu jeu)
{
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(jeu.bulleEncre[i].type == 0)
        {
            color(0,0,0,jeu.bulleEncre[i].lifetime*0.85);
            // On veut que la taille de la bulle d'encre augmente en 5 s jusqu'à une taille de 100 avec une taille initiale à 20.
            circleFill(jeu.bulleEncre[i].position.x,jeu.bulleEncre[i].position.y,100-jeu.bulleEncre[i].lifetime*0.267);
        }
    }
}

/*
    Jeu ------------------------------------------------------------------------
*/

//                          Actualisation/Fonctionnement

// Fonction pour initialiser un jeu
Jeu initialiserJeu(int etat) {
    Jeu jeu;

    jeu.etat = etat;
    // On défini l'attente de vague à false, l'actualisation du jeu au lancement créant un délai avant que la partie ne commence
    jeu.enAttenteDeVague = false;
    // On défini nVague à un pour la première vague, lorsqu'on démarre une partie
    jeu.nVague = 1;
    // Le score est défini à 0
    jeu.score = 0;

    jeu.nbCrabes = 0;

    jeu.sous = 0;

    // On défini les délais du jeu
    jeu.delaiDegats = elapsedTime();
    jeu.delaiTir = elapsedTime();

    // Initialisation du poulpe
    jeu.poulpe.vie = 50;
    jeu.poulpe.position = gener_vec2(500,500);
    jeu.poulpe.vitesse = gener_vec2(0,0);

    // Initialisation des images
    jeu.imgCrabe = image("data/Poulpy/crab.png");
    jeu.poulpe.image = image("data/Poulpy/octopus.png");

    initialisationEntitesJeu(jeu);
    //gener_vague_crabe(entites, nVague);
    return jeu;
}
// Fonction qui actualise un jeu complet
void actualiseJeu(Jeu &jeu)
{
    jeu.nbCrabes = 0;
    actualisePoulpe(jeu);
    for(int i = 0; i<MAX_ENTITES; i++)
    {
        if(jeu.crabes[i].type == 1)
            actualiseCrabe(jeu, i);
        if(jeu.bulleEncre[i].type == 0)
            actualiseBulleEncre(jeu, i);
        nettoyer(jeu.bulleEncre);
    }
    vague(jeu);
}

//                                      Affichage

// Fonction pour dessiner le jeu
void dessineJeu(Jeu jeu)
{
    dessineCrabes(jeu);
    dessinePoulpe(jeu);
    // On dessine les bulles après tous les autres pour qu'elles s'affichent sur les autres
    dessineBulles(jeu);
}

/*
    États ----------------------------------------------------------------------
*/

// Fonction pour affichage du menu principal et ses actions
void menuPrincipal(Image menu, int &etat, Image aide)
{
    image_draw(menu,0,0);
    int x,y;
    mousePos(x,y);
    color(183,238,255);
    print(5,5,"© Jean Pantaleon");
    if(isMousePressed(SDL_BUTTON_LEFT))
    {
        if(x>417 && x<862)
        {
            if(y>258 && y<376)
            {
                std::cout << "Jeu lancé !" << std::endl;
                etat = 1;
            } else if(y>124 && y < 232)
            {
                exit(0);
            }
        }
    }
    if(x > DIMX-64-15 && x < DIMX-15 && y > 15 && y < 64+15)
    {
        image_draw(aide, 0,0);
    }
}
// Fonction pour l'affichage du bouton pause durant la partie
void boutonPause(Image bouton, int &etat)
{
    image_draw(bouton, 1280-32, 720-32);
    int x,y;
    mousePos(x,y);
    if(isMousePressed(SDL_BUTTON_LEFT))
    {
        if(x>1280-32 && y >720-32)
        {
            etat = 2;
        }
    }
}
// Fonction pour l'affichage du menu de pause
void menuPause(Image pause, int &etat, Image aide){
    int time = elapsedTime();
    int valable = time%2;
    int x,y;
    mousePos(x,y);
    switch(valable)
    {
        case 0:
            image_draw(pause, 0,0);
            break;
        default:
            break;
    }
    if(isKeyPressed(SDLK_SPACE))
    {
        etat = 1;
    }
    if(x > DIMX-64-15 && x < DIMX-15 && y > 15 && y < 64+15)
    {
        image_draw(aide, 0,0);
    }
}
// Fonction pour affichage/gestion de la fin de jeu
void fin(Image ecranFin, Jeu &jeu){
    image_draw(ecranFin, 0,0);
    color(183,238,255);
    fontSize(56);
    print(695, 335, jeu.score);
    if(isKeyPressed(SDLK_SPACE))
    {
        jeu = initialiserJeu(1);
    }
}

/*
    Magasin --------------------------------------------------------------------
*/

//                        Actualisation/Fonctionnement

// Fonction qui gère la possibilité d'achats

void acheter(Jeu &jeu)
{
    if(isKeyPressed(SDLK_g))
    {
        if(jeu.poulpe.vie <= 45 && jeu.sous >= 3)
        {
            jeu.poulpe.vie += 5;
            jeu.sous -= 3;
            jeu.score -= 1;
        }
    }
}

// Fonction qui dessine le magasin
void dessineMagasin(Jeu jeu)
{
    color(183, 238,255);
    fontSize(20);
    print(15,15,"Pieces: ");
    print(90,15,jeu.sous);
}



/*
    Principal -------------------------------------------------------------------
*/

int main(int , char**)
{
	bool run;
	winInit("Pouply",DIMX,DIMY);
	backgroundColor( 18, 156, 235 );
	//initialisation(entites);

	//gener_vague_crabe(entites, nVague);

    /* On a ici les différents états du programme dans l'ordre d'apparition :
        0 -> Menu principal
        1 -> En jeu
        2 -> En pause
        3 -> fini
    */

    Image menu = image("data/Poulpy/menu.png");
	Image menupause = image("data/Poulpy/menupause.png");
    Image pause = image("data/Poulpy/pause.png");
    Image ecranFin = image("data/Poulpy/fin.png");
    Image aide = image("data/Poulpy/aide.png");

    // Initialisation de la structure Jeu
    Jeu jeu = initialiserJeu(0);

    float time = elapsedTime();

	do{
        winClear();
        switch(jeu.etat)
        {
            case 0:
                menuPrincipal(menu, jeu.etat, aide);
                break;
            case 1:
                if((elapsedTime() - time)*1000 > 17)
                {
                    actualiseJeu(jeu);
                    actionsPoulpe(jeu);
                    calculDegatPoulpe(jeu);
                    time = elapsedTime();
                }
                acheter(jeu);
                dessineJeu(jeu);
                dessineMagasin(jeu);
                boutonPause(pause, jeu.etat);
                color(255,255,255);
                fontSize(56);
                print(10,DIMY-70, jeu.score);
                break;
            case 2:
                dessineJeu(jeu);
                menuPause(menupause, jeu.etat, aide);
                break;
            case 3:
                fin(ecranFin, jeu);
                break;
            default:
                exit(1);
        }
        run = winDisplay();
	}while(!run);
	winQuit();
	return 0;
}
