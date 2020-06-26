// thread example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <curses.h>
#include <mutex>

const int numberOfPhilosophers = 5;

bool endOfProgram = false;

bool forks[5] = {true, true, true, true, true};

std::mutex screenLock;
std::mutex forkLock;

void writeOnScreen(int id, std::string str, int color)
{
    std::lock_guard<std::mutex> lock(screenLock);
    attron(COLOR_PAIR(color));
    mvaddstr(id, 0, str.c_str());
    refresh();

}

bool changeForkState(int id, int forkId, std::string whatDoYouDo)
{
    std::lock_guard<std::mutex> lock(forkLock);
    if(whatDoYouDo == "pickup")
    {
        if(forks[forkId])
        {
            forks[forkId] = false;
            return true;
        }
    }
    else if(whatDoYouDo == "drop")
    {
        if( ! forks[forkId])
        {
            forks[forkId] = true;
            return true;
        }
    }
    return false;
}

void executeThread(int id)
{
    std::string state = "thinking";
    std::string progressBar = "[      ]";
    std::string statePrint;
    int howManyTimesEating = 0;
    int forkPos1, forkPos2;
    bool doIHaveFork1 = false, doIHaveFork2 = false;
    std::string str;
    std::string food[10] = {"spaghetti","an apple","soup","sausages","bread","fries","chicken","kebab","sushi","lasagna"};
    std::string things[10] = {"heaven","my existence","god","life","you","the future", "the past", "eating", "nature","nothing"};
    auto printData = [](std::string food[], std::string things[], std::string statePrint, std::string state, std::string str, int forkPos1, int forkPos2, bool doIHaveFork1, int doIHaveFork2, int howManyTimesEating, int id) 
    { 
        move(id, 0);
        clrtoeol(); 
        int colorType = 3;
        statePrint = state;
        if(state == "eating")
        {
            statePrint += "...";
            colorType = 2;
        }
        statePrint.resize(27,' ');
        str = "My forks are: " + std::to_string(forkPos1) + ", " + std::to_string(forkPos2)
        + ". Do i have forks? " + std::to_string(doIHaveFork1) + " " + std::to_string(doIHaveFork2)
        + ". And i am " + statePrint + " I have eaten " + std::to_string(howManyTimesEating) +  " times.";

            writeOnScreen(id, str, colorType);
    };
    auto sleepFun = [](std::string food[], std::string things[], std::string statePrint, std::string state, std::string str, int forkPos1, int forkPos2, bool doIHaveFork1, int doIHaveFork2, int howManyTimesEating, int id)
    {
        move(id, 0);
        clrtoeol(); 
        int colorType = 3;
        statePrint = state;
        if(state == "eating")
        {
            statePrint += " " + food[std::rand() % 10];
            colorType = 2;
        }
        else if(state == "thinking")
        {
            statePrint += " about " + things[std::rand() % 10];
            colorType = 1;
        }
        statePrint.resize(27,' ');
        str = "My forks are: " + std::to_string(forkPos1) + ", " + std::to_string(forkPos2)
        + ". Do i have forks? " + std::to_string(doIHaveFork1) + " " + std::to_string(doIHaveFork2)
        + ". And i am " + statePrint + " I have eaten " + std::to_string(howManyTimesEating) +  " times.";
        writeOnScreen(id, str, colorType);
        usleep(500000);
        str += " ";
        for(int i = 0; i < 5; i++)
        {
            str += std::to_string((int)(100*(i/7.0))) + "\%";
            writeOnScreen(id, str, colorType);
            str = str.substr(0, str.length() - 3);
            usleep(400000);
        }
        str += std::to_string((int)(100*(6/7.0))) + "\%";
        writeOnScreen(id, str, colorType);
        str = str.substr(0, str.length() - 3);
        usleep(700000 * (rand() % 100)/100.0);
        str += std::to_string((int)(100*(7/7.0))) + "\%";
        writeOnScreen(id, str, colorType);
        str = str.substr(0, str.length() - 3);
        usleep(700000 * (rand() % 100)/100.0);
    };

    while(true)
    {
        forkPos1 = id;
        forkPos2 = (id + (5+1)) % 5;

        // Próba podniesienia obu widelców
        do{
            if(forks[forkPos1])
            {
                if(changeForkState(id, forkPos1, "pickup")) doIHaveFork1 = true;
            }
            if(forks[forkPos2])
            {
                if(changeForkState(id, forkPos2, "pickup")) doIHaveFork2 = true;
            }
            // Sprawdzenie czy ma sie oba widelce
            if(doIHaveFork1 && doIHaveFork2)
            {
                state = "eating";
            }
            else
            {
                if(doIHaveFork1){ changeForkState(id, forkPos1, "drop"); doIHaveFork1 = false;}
                if(doIHaveFork2){ changeForkState(id, forkPos2, "drop"); doIHaveFork2 = false;}
                state = "waiting for forks";
            }

            if(endOfProgram) break;
            printData(food, things, statePrint, state, str, forkPos1, forkPos2, doIHaveFork1, doIHaveFork2, howManyTimesEating, id);
            usleep(1000000 + 1000000 * (rand() % 100)/100.0 ); // Czekanie na sztucce 1-2s
        }while(state != "eating");
        if(endOfProgram) break;
        //printData(food, things, statePrint, state, str, forkPos1, forkPos2, doIHaveFork1, doIHaveFork2, howManyTimesEating, id);

        if(endOfProgram) break;
        sleepFun(food, things, statePrint, state, str, forkPos1, forkPos2, doIHaveFork1, doIHaveFork2, howManyTimesEating, id);
        //usleep(2500000 + 1500000 * (rand() % 100)/100.0 ); // Jedzenie 2,5-4s

        howManyTimesEating++;
        //Odlozenie sztuccow
        changeForkState(id, forkPos1, "drop"); doIHaveFork1 = false;
        changeForkState(id, forkPos2, "drop"); doIHaveFork2 = false;
        state = "thinking";

        if(endOfProgram) break;
        sleepFun(food, things, statePrint, state, str, forkPos1, forkPos2, doIHaveFork1, doIHaveFork2, howManyTimesEating, id);
        //usleep(2500000 + 1500000 * (rand() % 100)/100.0 ); // Myslenie 2,5-4s


    }
    //clear
    if(doIHaveFork1)changeForkState(id, forkPos1, "drop");
    if(doIHaveFork2)changeForkState(id, forkPos2, "drop");
    move(id, 0);          // move to begining of line
    clrtoeol(); 
    writeOnScreen(id, "Completed", 2);
}

void checkForExit()
{
    while (1)
    {
        //cbreak();
        getch();
        endOfProgram = true;
        writeOnScreen(7, "checkForExit thread ended", 2);
        return;
    }
    
}

int main(int argc, char* argv[]) 
{
    WINDOW * mainwin;
    
    if ( (mainwin = initscr()) == NULL )
    {
        fprintf(stderr, "Error initialising ncurses.\n");
        exit(0);
    }
    // kolorki
    if (has_colors() == FALSE)
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_GREEN);
    init_pair(3, COLOR_BLACK, COLOR_RED);
    curs_set(0);

    srand (time(NULL));
    std::thread threads[numberOfPhilosophers];
    std::thread checkForExitThread = std::thread(checkForExit);
    checkForExitThread.detach();

    for(int i = 0; i < numberOfPhilosophers; i++)
    {
        threads[i] = std::thread(executeThread, i);
    }
    for(int i = 0; i < numberOfPhilosophers; i++)
    {
        threads[i].join();
    }
delwin(mainwin);
endwin();
refresh();
std::cout<< "All threads ended\n";
return 0;
}