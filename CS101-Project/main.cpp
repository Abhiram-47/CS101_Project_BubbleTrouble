#include <simplecpp>
#include <sstream>
#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int RIGHT_MARGIN = 430;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);
const int MAX_HEALTH = 3;

int SCORE = 0;


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(255,105,180)));
    return bubbles;
}


bool shooterShot(vector<Bubble> &bubbles, Shooter &shooter) {
    for (unsigned int i=0; i<bubbles.size(); i++) {

        double circle_x = bubbles[i].get_center_x();
        double circle_y = bubbles[i].get_center_y();
        double circle_r = bubbles[i].get_radius();

        double circle_head = (circle_x - shooter.get_head_center_x())*(circle_x - shooter.get_head_center_x())
                           + (circle_y - shooter.get_head_center_y())*(circle_y - shooter.get_head_center_y());

        if (circle_head <= (circle_r + shooter.get_head_radius())*
                           (circle_r + shooter.get_head_radius())) {
            shooter.shot();
            bubbles[i].nextStep(0.3);
            return true;
        }
        
        double width  = shooter.get_body_width();
        double height = shooter.get_body_height();
        double center_x = shooter.get_body_center_x();
        double center_y = shooter.get_body_center_y();
            
        double centerDistx = abs(circle_x - center_x);
        double centerDisty = abs(circle_y - center_y);

        if (centerDistx > (width/2 + circle_r)) { continue; }
        if (centerDisty > (height/2 + circle_r)) { continue; }

        if (centerDistx <= (width/2)) { shooter.shot(); bubbles[i].nextStep(0.3); return true;} 
        if (centerDisty <= (height/2)) { shooter.shot(); bubbles[i].nextStep(0.3); return true;}

        double cornerDistance_sq = (centerDistx - width/2)*(centerDistx - width/2) +
                                   (centerDisty - height/2)*(centerDisty - height/2);
        
        if (cornerDistance_sq <= (circle_r*circle_r)) {
            shooter.shot();
            bubbles[i].nextStep(0.3);
            return true;
        }
    }
    return false;
}


void removeIntersect(vector<Bubble> &bubbles, vector<Bullet> &bullets) {
    for (unsigned int i=0; i<bubbles.size(); i++) {
        double circle_x = bubbles[i].get_center_x();
        double circle_y = bubbles[i].get_center_y();
        double circle_r = bubbles[i].get_radius();

        for (unsigned int j=0; j<bullets.size(); j++) {

            double width  = bullets[j].get_width();
            double height = bullets[j].get_height();
            double center_x = bullets[j].get_center_x();
            double center_y = bullets[j].get_center_y();

            double centerDistx = abs(circle_x - center_x);
            double centerDisty = abs(circle_y - center_y);

            if (centerDistx > (width/2 + circle_r)) { continue; /*return false;*/ }
            if (centerDisty > (height/2 + circle_r)) { continue; /*return false;*/ }

            if (centerDistx <= (width/2)) { /*return true;*/ 

                SCORE++;
                bubbles.erase(bubbles.begin() + i); 
                bullets.erase(bullets.begin() + j); 
                break;
            } 
            if (centerDisty <= (height/2)) { /*return true;*/

                SCORE++;
                bubbles.erase(bubbles.begin() + i); 
                bullets.erase(bullets.begin() + j); 
                break;            
            }

            double cornerDistance_sq = (centerDistx - width/2)*(centerDistx - width/2) +
                                 (centerDisty - height/2)*(centerDisty - height/2);

            if (cornerDistance_sq <= (circle_r*circle_r)) {
            
                SCORE++;
                bubbles.erase(bubbles.begin() + i); 
                bullets.erase(bullets.begin() + j); 
                break; 
            }
        }
    }
}


int main()
{

    double Time = 50;

    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));
    
    string msg_cmd("Cmd: _");
    string hlth_msg("Health:  3/3");
    string score_msg("Score: ");
    string time("Time: ");

    Text timeMsg(LEFT_MARGIN, TOP_MARGIN, time);
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);
    Text HCntr(RIGHT_MARGIN, BOTTOM_MARGIN, hlth_msg);
    Text Score((LEFT_MARGIN + RIGHT_MARGIN)/2.0, BOTTOM_MARGIN, score_msg);


    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    // Main game loop
    int i_t=0;
    while (true)
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {   
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);
            
            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }
        
        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        if (shooterShot(bubbles, shooter)) {
            if (shooter.get_health()==0) {
                stringstream str1;
                str1 << shooter.get_health();
                hlth_msg[hlth_msg.length() - 3] = str1.str()[0];
                HCntr.setMessage(hlth_msg);
                Text gameOver(WINDOW_X/2, WINDOW_Y/2, "Game Over, You Lose!");
                wait(10);
                break;
            }
        }



        stringstream str1, str2, str3;
        str1 << shooter.get_health();
        str2 << SCORE;
        str3 << int(Time);

        hlth_msg[hlth_msg.length() - 3] = str1.str()[0];
        HCntr.setMessage(hlth_msg);
        Score.setMessage(score_msg + str2.str());

        if  ( i_t%50 == 0 ) {timeMsg.setMessage(time + str3.str()); }

        if (i_t == 2500) {
            Text gameOver(WINDOW_X/2, WINDOW_Y/2, "Game Over, You Lose!");
            wait(10);
            break;
        }



        removeIntersect(bubbles, bullets);

        if (bubbles.size() == 0) {
            Text won(WINDOW_X/2, WINDOW_Y/2, "Congrats!");
            wait(3);

        }

        Time = Time - STEP_TIME;
        i_t += 1;
        wait(STEP_TIME);

    }


}
