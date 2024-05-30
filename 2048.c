#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// numesc o casuta din joc ce are o anumita culoare si o valoare putere a lui 2
// o "piesa"

// verific daca piesele salvate in memorie sunt valide
int validpiece(int x) {
  int check[12] = {0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048}, i,
      valid = 0;
  for (i = 0; i < 12; i++) {
    if (x == check[i]) {
      valid = 1;
      break;
    }
  }
  return valid;
}

// transform piesele din inturi in charuri ca sa le pot salva in memorie ca un
//  mare string
void inttochar(int x, char ch[6]) {
  ch[4] = ' ';
  ch[5] = '\0';
  if (x == 2048) {
    ch[0] = '2';
    ch[1] = '0';
    ch[2] = '4';
    ch[3] = '8';
    return;
  }
  if (x == 1024) {
    ch[0] = '1';
    ch[1] = '0';
    ch[2] = '2';
    ch[3] = '4';
    return;
  }
  ch[3] = ' ';
  if (x > 100) {
    ch[2] = '0' + x % 10;
    ch[1] = '0' + x / 10 % 10;
    ch[0] = '0' + x / 100;
    return;
  }
  ch[2] = ' ';
  if (x > 10) {
    ch[1] = '0' + x % 10;
    ch[0] = '0' + x / 10;
    return;
  }
  ch[1] = ' ';
  ch[0] = '0' + x;
  return;
}

// returnez codul perechii de culori pentru o anumita piesa
int colorsquare(int x) {
  if (x == 0) {
    return 0;
  }
  if (x == 2) {
    return 2;
  }
  if (x == 4) {
    return 3;
  }
  if (x == 8) {
    return 4;
  }
  if (x == 16) {
    return 5;
  }
  if (x == 32) {
    return 6;
  }
  if (x == 64) {
    return 7;
  }
  if (x == 128) {
    return 8;
  }
  if (x == 256) {
    return 9;
  }
  if (x == 512) {
    return 10;
  }
  if (x == 1024) {
    return 11;
  }
  if (x == 2048) {
    return 12;
  }
  return 0;
}

// pun piese pe tabla la intamplare
void spawn(int board[4][4], int order) {
  // order apare pentru ca uneori trebuie sa creez doua piese distincte in
  // aceeasi secunda
  srand(time(NULL) + order);

  // retin spatiile libere unde pot pune o piesa
  int free[16] = {0}, nbfree = 0, i, j;
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (board[i][j] == 0) {
        free[nbfree++] = 4 * i + j;
      }
    }
  }
  if (nbfree == 0) {
    return;
  }
  int pos = rand() % nbfree, value = rand() % 2;
  board[free[pos] / 4][free[pos] % 4] = 2 + 2 * value;
}

// verific daca pe orizontala sau verticala se pot elibera spatii
int checkfree(int board[4][4], int way) {
  int i, j, k, freed = 0;
  if (way == 0) {
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 3; j++) {
        if (board[i][j]) {
          for (k = j + 1; k < 4; k++) {
            // daca doua piese au aceeasi valoare, ele
            // pot fi combinate, eliberand 1 spatiu
            if (board[i][k] == board[i][j]) {
              freed++;
              break;
            } else if (board[i][k] != 0) {
              break;
            }
          }
          j = k - 1;
        }
      }
    }
  } else {
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 3; j++) {
        if (board[j][i]) {
          for (k = j + 1; k < 4; k++) {
            // daca doua piese au aceeasi valoare, ele
            // pot fi combinate, eliberand 1 spatiu
            if (board[k][i] == board[j][i]) {
              freed++;
              break;
            } else if (board[k][i] != 0) {
              break;
            }
          }
          j = k - 1;
        }
      }
    }
  }
  return freed;
}

// realizez logica unei mutari, verificand totodata daca aceasta rezulta
// intr-o victorie si daca este valida
int movement(int board[4][4], char mv, int *score, int goal) {
  // in valid verific daca mutarea e valida, adica daca produce o schimbare pe
  // tabla (caz in care valid devine nenul) si in win retin daca a fost creata o
  // piesa castigatoare
  int start = 3, end = 0, step = -1, i, j, k, valid = 0, win = 0;

  // aleg sus si stanga ca sensul "pozitiv". Verific daca mutarea are loc in
  // sens pozitiv sau negativ, si modific startul, finalul si pasul ca atare
  if (mv == 'u' || mv == 'l') {
    start = 0;
    end = 3;
    step = 1;
  }

  // verific daca mutarea are loc pe orizontala sau verticala
  if (mv == 'u' || mv == 'd') {
    for (i = 0; i < 4; i++) {
      for (j = start; j != end; j += step) {
        // cand gasesc un 0, caut o piesa nenula pe care sa o pun in
        // locul sau
        if (board[j][i] == 0) {
          for (k = j + step; k != end + step; k += step) {
            if (board[k][i] != 0) {
              board[j][i] = board[k][i];
              board[k][i] = 0;
              valid++;
              break;
            }
          }
        }
      }
      // verific daca 2 piese aflate una langa alta pe directia mutarii
      // sunt egale. Daca da, le combin si umplu locul liber cu celelalte
      // piese
      for (j = start; j != end; j += step) {
        if (board[j][i] == board[j + step][i] && board[j][i] != 0) {
          board[j][i] *= 2;

          // verific daca aceasta combinare rezulta intr-o victorie
          if (board[j][i] >= goal) {
            win = 1;
          }
          *score += board[j][i];
          board[j + step][i] = 0;
          valid++;
          for (k = j + step; k != end; k += step) {
            board[k][i] = board[k + step][i];
            board[k + step][i] = 0;
          }
        }
      }
    }
  } else {
    for (i = 0; i < 4; i++) {
      // verific daca 2 piese aflate una langa alta pe directia mutarii
      // sunt egale. Daca da, le combin si umplu locul liber cu celelalte
      // piese
      for (j = start; j != end; j += step) {
        if (board[i][j] == 0) {
          for (k = j + step; k != end + step; k += step) {
            if (board[i][k] != 0) {
              board[i][j] = board[i][k];
              board[i][k] = 0;
              valid++;
              break;
            }
          }
        }
      }
      for (j = start; j != end; j += step) {
        if (board[i][j] == board[i][j + step] && board[i][j] != 0) {
          board[i][j] *= 2;

          // verific daca aceasta combinare rezulta intr-o victorie
          if (board[i][j] >= goal) {
            win = 1;
          }
          valid++;
          *score += board[i][j];
          board[i][j + step] = 0;
          for (k = j + step; k != end; k += step) {
            board[i][k] = board[i][k + step];
            board[i][k + step] = 0;
          }
        }
      }
    }
  }
  if (!valid) {
    return -1;
  }
  if (win) {
    return goal;
  }
  return valid;
}

// fac o mutare automata
int automove(int board[4][4], int *score, int goal) {
  int freedx = 0, freedy = 0, win = 0;

  // verific pe ce directie ar fi eliberate cele mai multe spatii
  freedx = checkfree(board, 0);
  freedy = checkfree(board, 1);

  // daca nicio mutare nu elibeareaza spatiu, le iau la rand
  if (freedx == 0 && freedy == 0) {
    // in win tin minte rezultatul mutarii. Daca e -1 (invalida), trec peste
    // Asta ma ajuta in cazul in care mutarea facuta automat e castigatoare
    win = movement(board, 'l', score, goal);
    if (win != -1) {
      spawn(board, 0);
    } else {
      win = movement(board, 'd', score, goal);
      if (win != -1) {
        spawn(board, 0);
      } else {
        win = (movement(board, 'r', score, goal));
        if (win != -1) {
          spawn(board, 0);
        } else {
          win = movement(board, 'u', score, goal);
        }
        if (win != -1) {
          spawn(board, 0);
        }
      }
    }
  } else if (freedx <= freedy) {
    win = movement(board, 'u', score, goal);
    spawn(board, 0);
  } else {
    win = movement(board, 'l', score, goal);
    spawn(board, 0);
  }
  return win;
}

// desenez ecranul meniului
void draw_menu(int maxy, int maxx, int selected, int resume) {
  clear();
  refresh();
  box(stdscr, 0, 0);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);

  // am 4 butoane, new game, resume, quit, setari
  WINDOW *resumebtn, *newgamebtn, *quitbtn, *settingsbtn;
  resumebtn = newwin(1, 10, maxy / 2 - 2, maxx / 2 - 5);
  newgamebtn = newwin(1, 12, maxy / 2 - 3, maxx / 2 - 6);
  quitbtn = newwin(1, 8, maxy / 2 - 1, maxx / 2 - 4);
  settingsbtn = newwin(1, 12, maxy / 2, maxx / 2 - 6);

  // in functie de ce alege jucatorul, scoatem un buton in evidenta
  if (selected == 0) {
    wattron(newgamebtn, A_STANDOUT);
    wprintw(newgamebtn, "* new game  ");
    wattroff(newgamebtn, A_STANDOUT);
  } else {
    wprintw(newgamebtn, "  new game  ");
  }
  if (selected == 1 && resume == 1) {
    wattron(resumebtn, COLOR_PAIR(1));
    wattron(resumebtn, A_STANDOUT);
    mvwprintw(resumebtn, 0, 0, "* resume  ");
    wattroff(resumebtn, A_STANDOUT);
    wattroff(resumebtn, COLOR_PAIR(1));
  } else if (resume == 1) {
    wattron(resumebtn, COLOR_PAIR(1));
    mvwprintw(resumebtn, 0, 0, "  resume  ");
    wattroff(resumebtn, COLOR_PAIR(1));
  }
  // daca nu avem jocuri in memorie, resume va fi 0 si vom colora altfel
  // butonul resume
  else {
    init_pair(14, COLOR_CYAN, COLOR_BLACK);
    wattron(resumebtn, COLOR_PAIR(14));
    mvwprintw(resumebtn, 0, 0, "  resume  ");
    wattroff(resumebtn, COLOR_PAIR(14));
  }
  if (selected == 2) {
    wattron(quitbtn, A_STANDOUT);
    wprintw(quitbtn, "* quit  ");
    wattroff(quitbtn, A_STANDOUT);
  } else {
    wprintw(quitbtn, "  quit  ");
  }
  if (selected == 3) {
    wattron(settingsbtn, A_STANDOUT);
    wprintw(settingsbtn, "* settings  ");
    wattroff(settingsbtn, A_STANDOUT);
  } else {
    wprintw(settingsbtn, "  settings  ");
  }
  refresh();
  wrefresh(resumebtn);
  wrefresh(newgamebtn);
  wrefresh(quitbtn);
  wrefresh(settingsbtn);
}

// aici este logica meniului
int menu(int maxy, int maxx, int resume) {
  int selected = 0, choice;
  choice = getch();

  // ne oprim la tasta enter
  while (choice != 10) {
    if (choice == KEY_UP) {
      selected--;
      if (selected == -1) {
        selected = 3;
      }
      // daca nu avem jocuri in memorie sarim peste resume
      else if (selected == 1 && resume == 0) {
        selected--;
      }
      draw_menu(maxy, maxx, selected, resume);
    } else if (choice == KEY_DOWN) {
      selected++;
      if (selected == 4) {
        selected = 0;
      }
      // daca nu avem jocuri in memorie sarim peste resume
      else if (selected == 1 && resume == 0) {
        selected++;
      }
      draw_menu(maxy, maxx, selected, resume);
    } else if (choice == '1') {
      selected = 0;
      draw_menu(maxy, maxx, selected, resume);
    } else if (choice == '2' && resume == 1) {
      selected = 1;
      draw_menu(maxy, maxx, selected, resume);
    } else if (choice == '3') {
      selected = 2;
      draw_menu(maxy, maxx, selected, resume);
    } else if (choice == '4') {
      selected = 3;
      draw_menu(maxy, maxx, selected, resume);
    }

    choice = getch();
  }
  return selected;
}

// desenez ecranul setarilor
void draw_settings(int maxy, int maxx, int selected, int option[2]) {
  // optiunile jucatorului
  char settingbot[5][22] = {"   Lightning    ", "      Hawk      ",
                            "      Human     ", "      Snail     ",
                            " Plate tectonics "};
  char settingdiff[5][22] = {" Training: 128  ", "   Easy: 256    ",
                             "   Medium: 512  ", "Half-price: 1024",
                             "Full game: 2048 "};
  clear();
  refresh();
  box(stdscr, 0, 0);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  WINDOW *box[2];

  // am doua butoane pentru 2 setari
  box[0] = newwin(1, 30, maxy / 2 - 1, maxx / 2 - 18);
  box[1] = newwin(1, 30, maxy / 2, maxx / 2 - 18);
  if (selected == 0) {
    wattron(box[0], A_STANDOUT);
    wprintw(box[0], "Bot speed: %s", settingbot[option[0]]);
    wattroff(box[0], A_STANDOUT);
  } else {
    wprintw(box[0], "Bot speed: %s", settingbot[option[0]]);
  }
  if (selected == 1) {
    wattron(box[1], A_STANDOUT);
    wprintw(box[1], "Game goal: %s", settingdiff[option[1]]);
    wattroff(box[1], A_STANDOUT);
  } else {
    wprintw(box[1], "Game goal: %s", settingdiff[option[1]]);
  }
  refresh();
  wrefresh(box[0]);
  wrefresh(box[1]);
}

// logica setarilor
void settings(int maxy, int maxx, int options[2]) {
  int choice = 0, selected = 0;
  draw_settings(maxy, maxx, selected, options);
  choice = getch();

  // la q (quit) sau 10 (enter) ies
  while (choice != 'q' && choice != 10) {
    if (choice == KEY_UP) {
      selected--;
      if (selected == -1) {
        selected = 1;
      }
      draw_settings(maxy, maxx, selected, options);
    } else if (choice == KEY_DOWN) {
      selected++;
      if (selected == 4) {
        selected = 0;
      }
      draw_settings(maxy, maxx, selected, options);
    } else if (choice == '1') {
      selected = 0;
      draw_settings(maxy, maxx, selected, options);
    } else if (choice == '2') {
      selected = 1;
      draw_settings(maxy, maxx, selected, options);
    } else if (choice == '3') {
      selected = 2;
      draw_settings(maxy, maxx, selected, options);
    } else if (choice == '4') {
      selected = 3;
      draw_settings(maxy, maxx, selected, options);
    }

    // fiecare setare are 5 optiuni prin care utilizatorul se misca cu
    // dreapta-stanga
    else if (choice == KEY_RIGHT) {
      options[selected]++;
      if (options[selected] == 5) {
        options[selected] = 0;
      }
      draw_settings(maxy, maxx, selected, options);
    } else if (choice == KEY_LEFT) {
      options[selected]--;
      if (options[selected] == -1) {
        options[selected] = 4;
      }

      draw_settings(maxy, maxx, selected, options);
    }
    choice = getch();
  }
}

// realizez ecranul pentru cand castiga un jucator
void draw_win(int maxy, int maxx, int score) {
  clear();
  refresh();
  WINDOW *winscreen = newwin(2, 16, maxy / 2 - 1, maxx / 2 - 7);
  init_pair(42, COLOR_BLUE, COLOR_WHITE);
  wbkgd(winscreen, COLOR_PAIR(42));
  mvwprintw(winscreen, 0, 0, "  You win! :D");
  mvwprintw(winscreen, 1, 0, "  Score: %d", score);
  wrefresh(winscreen);
  getch();
  wclear(winscreen);
}

// realizez ecranul pentru cand pierde un jucator
void draw_lose(int maxy, int maxx, int score) {
  clear();
  refresh();
  WINDOW *losescreen = newwin(2, 14, maxy / 2 - 1, maxx / 2 - 7);
  init_pair(13, COLOR_WHITE, COLOR_RED);
  wbkgd(losescreen, COLOR_PAIR(13));
  mvwprintw(losescreen, 0, 0, " You lose! :(");
  mvwprintw(losescreen, 1, 0, " Score: %d", score);
  wrefresh(losescreen);
  getch();
  wclear(losescreen);
}

// desenez ecranul pentru jocul efectiv
void draw_game(int board[4][4], int maxy, int maxx, int score, int goal) {
  // tin minte ora si data curenta
  time_t timenow;
  time(&timenow);
  struct tm *timestruct = localtime(&timenow);
  int hour = timestruct->tm_hour;
  int minute = timestruct->tm_min;
  int day = timestruct->tm_mday;
  int month = timestruct->tm_mon + 1;
  int year = timestruct->tm_year + 1900;
  int i, j;
  clear();

  // realizez culorile si perechile de culori pentru piese
  init_color(12, 800, 600, 100);
  init_color(13, 400, 800, 100);
  init_color(14, 100, 800, 500);
  init_color(15, 100, 800, 800);
  init_color(16, 200, 600, 1000);
  init_color(17, 200, 200, 1000);
  init_color(18, 0, 0, 600);
  init_color(19, 600, 200, 1000);
  init_color(110, 1000, 200, 1000);
  init_color(111, 1000, 200, 600);
  init_color(112, 1000, 200, 200);
  init_pair(20, COLOR_BLACK, COLOR_WHITE);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, 12);
  init_pair(3, COLOR_WHITE, 13);
  init_pair(4, COLOR_WHITE, 14);
  init_pair(5, COLOR_WHITE, 15);
  init_pair(6, COLOR_WHITE, 16);
  init_pair(7, COLOR_WHITE, 17);
  init_pair(8, COLOR_WHITE, 18);
  init_pair(9, COLOR_WHITE, 19);
  init_pair(10, COLOR_WHITE, 110);
  init_pair(11, COLOR_WHITE, 111);
  init_pair(12, COLOR_WHITE, 112);
  bkgdset(1);

  // declar "corpul" pieselor ca vector de ferestre
  WINDOW *cell[16], *sidebar;

  // fac sidebarul cu tot ce e in cerinta, dar si cu punctajul necesar pentru
  // victorie
  sidebar = newwin(13, 15, maxy / 2 - 8, maxx / 2 + 18);
  box(sidebar, 0, 0);
  wbkgd(sidebar, COLOR_PAIR(20));
  if (day < 10) {
    mvwprintw(sidebar, 1, 1, "0%d.", day);
  } else {
    mvwprintw(sidebar, 1, 1, "%d.", day);
  }
  if (month < 10) {
    wprintw(sidebar, "0");
  }
  wprintw(sidebar, "%d.%d ", month, year);
  if (hour < 10) {
    mvwprintw(sidebar, 2, 1, "0%d:", hour);
  } else {
    mvwprintw(sidebar, 2, 1, "%d:", hour);
  }

  if (minute < 10) {
    wprintw(sidebar, "0");
  }
  wprintw(sidebar, "%d", minute);
  mvwprintw(sidebar, 3, 1, "Score: %d", score);
  mvwprintw(sidebar, 4, 1, "Controls:");
  mvwprintw(sidebar, 5, 1, "Up arrow");
  mvwprintw(sidebar, 6, 1, "Right arrow");
  mvwprintw(sidebar, 7, 1, "Left arrow");
  mvwprintw(sidebar, 8, 1, "Down arrow");
  mvwprintw(sidebar, 9, 1, "Q: quit game");
  mvwprintw(sidebar, 10, 1, "Any key: Auto");
  mvwprintw(sidebar, 11, 1, "Goal: %d", goal);

  // asez toate piesele
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      cell[4 * i + j] =
          newwin(3, 6, maxy / 2 - 8 + 4 * i, maxx / 2 - 14 + 8 * j);
      box(cell[4 * i + j], 0, 0);
    }
  }

  // colorez piesele in functie de cat de mari sunt numerele din ele, ca sa stiu
  // de cate spatii e nevoie
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (board[i][j] < 100) {
        wattron(cell[4 * i + j], COLOR_PAIR(colorsquare(board[i][j])));
        mvwprintw(cell[4 * i + j], 0, 0, "      ");
        mvwprintw(cell[4 * i + j], 1, 0, "  %d  ", board[i][j]);
        mvwprintw(cell[4 * i + j], 2, 0, "      ");
        if (board[i][j] < 10) {
          mvwprintw(cell[4 * i + j], 1, 5, " ");
        }
        wattroff(cell[4 * i + j], COLOR_PAIR(colorsquare(board[i][j])));
      } else {
        wattron(cell[4 * i + j], COLOR_PAIR(colorsquare(board[i][j])));
        mvwprintw(cell[4 * i + j], 0, 0, "      ");
        mvwprintw(cell[4 * i + j], 2, 0, "      ");
        mvwprintw(cell[4 * i + j], 1, 0, " %d ", board[i][j]);
        if (board[i][j] < 1000) {
          mvwprintw(cell[4 * i + j], 1, 5, " ");
        }
        wattroff(cell[4 * i + j], COLOR_PAIR(colorsquare(board[i][j])));
      }
    }
  }
  refresh();
  for (i = 0; i < 16; i++) {
    wrefresh(cell[i]);
  }
  wrefresh(sidebar);
}

// logica jocului
void game(int board[4][4], int maxy, int maxx, int delay, int goal) {
  int input = 0, i, j, newgame = 1, score = 0, mvres = 0, automv = 1;

  // pun timp plimita la cat poate sa stea utilizatorul fara sa faca o mutare
  timeout(1000);

  // verific daca jocul este nou, daca da, pun pe ecran 2 piese
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (board[i][j] != 0) {
        newgame = 0;
      }
    }
  }
  if (newgame) {
    spawn(board, 0);
    spawn(board, 1);
  }

  // joc pana la q (quit)
  while (input != 'q') {
    draw_game(board, maxy, maxx, score, goal);

    // daca nu exista piese care se pot combina, verific daca tabla e plina
    // Daca tabla e plina retin codul pentru pierdere si ies din while
    if (checkfree(board, 0) == 0 && checkfree(board, 1) == 0) {
      mvres = -2048;
      for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
          if (board[i][j] == 0) {
            mvres = 0;
            break;
          }
        }
        if (mvres == 0) {
          break;
        }
      }
      if (mvres == -2048) {
        break;
      }
    }

    // in automv retin daca trebuie sa se faca o mutare automat. Daca una
    // manuala e facuta variabila devine 0
    automv = 1;
    input = 0;

    // aici astept un interval de timp numar natural sa se faca o mutare
    // desenez din nou fereastra in caz ca s-a schimbat minutul/ora/data
    // daca setarea delay e 0, miscarile automate vor avea loc aproape
    // instantaneu
    for (i = 0; i < delay; i++) {
      input = getch();
      draw_game(board, maxy, maxx, score, goal);
      if (input == KEY_DOWN || input == KEY_UP || input == KEY_RIGHT ||
          input == KEY_LEFT || input == 'q') {
        break;
      }
    }

    // in mvres retin rezultatul unei mutari, adica daca e valida, daca a dus la
    // o victorie. Daca e valida mai adaug o piesa si daca e castigatoare termin
    if (input == KEY_UP) {
      mvres = movement(board, 'u', &score, goal);
      if (mvres == goal) {
        break;
      }
      if (mvres != -1) {
        spawn(board, 0);
      }
      automv = 0;
    } else if (input == KEY_RIGHT) {
      mvres = movement(board, 'r', &score, goal);
      if (mvres == goal) {
        break;
      }
      if (mvres != -1) {
        spawn(board, 0);
      }
      automv = 0;
    } else if (input == KEY_LEFT) {
      mvres = movement(board, 'l', &score, goal);
      if (mvres == goal) {
        break;
      }
      if (mvres != -1) {
        spawn(board, 0);
      }
      automv = 0;
    } else if (input == KEY_DOWN) {
      mvres = movement(board, 'd', &score, goal);
      if (mvres == goal) {
        break;
      }
      if (mvres != -1) {
        spawn(board, 0);
      }
      automv = 0;
    }

    // daca nu a avut loc o mutare valida si jucatorul nu a incercat sa iasa,
    // fac o mutare automat. Jucatorul poate sa accelereze botul, facandu-l cu
    // orice tasta sa faca o miscare
    if (automv && input != 'q') {
      mvres = automove(board, &score, goal);
      if (mvres == goal) {
        break;
      } else if (mvres == -2048) {
        break;
      }
    }
  }
  draw_game(board, maxy, maxx, score, goal);
  timeout(300000);

  // verific daca jocul a fost pierdut sau castigat, resetez tabla de joc
  if (mvres == goal) {
    getch();
    draw_win(maxy, maxx, score);
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        board[i][j] = 0;
      }
    }
  }
  if (mvres == -2048) {
    getch();
    draw_lose(maxy, maxx, score);
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        board[i][j] = 0;
      }
    }
  }
}

int main() {
  initscr();

  // fac calculatorul sa poata primi sagetile ca input
  keypad(stdscr, TRUE);
  raw();

  // ce scrie utilizatorul nu apare pe ecran
  noecho();

  // sa mearga ctrl c
  cbreak();

  // sa nu apare cursorul
  curs_set(0);

  // pornesc culorile
  start_color();

  // maxx si maxy sunt dimensiunile ecranului jucatorului, resume retine daca e
  // un joc in memorie, set retine setarile, hasmemory retine daca avem memorie
  // in fisier si setlen este lungimea datelor ce apar in memorie
  int maxx, maxy, selected, board[4][4] = {0}, resume = 0, i, j, k,
                            set[2] = {2, 4}, hasmemory = 1, setlen;

  // aici retinem valorile schimbabile in setari: numarul pana la care se joaca
  // si viteza miscarilor automate
  int goal[5] = {128, 256, 512, 1024, 2048};
  int botspeed[5] = {0, 1, 5, 10, 50};
  char memory[100], save[100], saveaux[6];
  getmaxyx(stdscr, maxy, maxx);
  refresh();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));

  // iau ce se afla in memorie in fisier. Fiecare informatie are dimensiunea 5,
  // fiind formata dintr-un numar si atatea spatii astfel incat sa se ajunga la
  // 5 caractere
  FILE *gamestate;
  gamestate = fopen("gamememory.txt", "r+");
  if (gamestate != NULL) {
    if (fgets(memory, 100, gamestate) != NULL) {
      // avem grija sa nu depasim lungimea sirului de caractere
      setlen = strlen(memory);
      k = 0;
      for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
          while (memory[k] != ' ' && k < setlen) {
            board[i][j] *= 10;
            board[i][j] += memory[k++] - '0';

            // daca in memorie piesa nu e valida, o inlocuim cu 0
            if (!validpiece(board[i][j])) {
              board[i][j] = 0;
            }
          }

          // trecem de spatiile de umplutura
          while (memory[k] == ' ' && k < setlen) {
            k++;
          }
        }
      }

      // ultimele doua numere sunt setarile. Le egalam cu 4 daca nu sunt valide
      if (k < setlen) {
        set[0] = memory[k++] - '0';
        if (set[0] > 4 || set[0] < 0) {
          set[0] = 4;
        }

        // trecem de spatiile de umplutura
        while (memory[k] == ' ' && k < setlen) {
          k++;
        }
        if (k < setlen) {
          set[1] = memory[k] - '0';
          if (set[1] > 4 || set[1] < 0) {
            set[1] = 4;
          }
        }
      }
    }
  } else {
    // daca nu avem memorie nu mai incercam sa scriem ceva la final
    hasmemory = 0;
  }
  // incep interactiunea cu jucatorul
  while (1) {
    resume = 0;

    // verific daca exista un joc inceput
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        if (board[i][j]) {
          resume = 1;
        }
      }
    }
    draw_menu(maxy, maxx, 0, resume);

    // vad ce a selectat jucatorul in meniu
    selected = menu(maxy, maxx, resume);
    if (selected == 0) {
      // resetez tabla de joc in caz ca se alege joc nou
      for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
          board[i][j] = 0;
        }
      }

      // incep jocul, tinand cont de setarile alese
      game(board, maxy, maxx, botspeed[set[0]], goal[set[1]]);
    } else if (selected == 1) {
      game(board, maxy, maxx, botspeed[set[0]], goal[set[1]]);
    } else if (selected == 2) {
      break;
    } else if (selected == 3) {
      settings(maxy, maxx, set);
    }
  }

  // daca avem acces la memorie, salvam in memorie tabla curenta si setarile
  if (hasmemory) {
    save[0] = '\0';
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        // transform valorile din tabla in stringuri pe care le concatenez
        inttochar(board[i][j], saveaux);
        strcat(save, saveaux);
      }
    }
    inttochar(set[0], saveaux);
    strcat(save, saveaux);
    inttochar(set[1], saveaux);
    strcat(save, saveaux);

    // am grija sa incep sa scriu de la inceput
    fseek(gamestate, 0, SEEK_SET);
    fputs(save, gamestate);
    fclose(gamestate);
  }
  endwin();
  return 0;
}