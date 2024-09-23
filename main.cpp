/*
Created by Jippe Heijnen on 16-09-2024.

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <thread>
#include <mutex>
#include <random>

using namespace std;

class stickman {
private:
    //randomness for blinking and moods
    random_device rd;
    mt19937 gen = mt19937(rd());


    mutex mood_mtx;
    mutex eye_mtx;

    // states
    enum class states {
        // emotion states
        angry,
        neutral,
        happy,

        // eye states
        closed
    };

    states mood_state = states::neutral;
    states eye_state = states::neutral;

    // visible parts
    string eyebrows;
    string eyes;

    // threads
    thread print_stickman = thread([&]()->void {
        while (true) {
            // sleep a second between prints
            auto sleep_time_ms = 10;

            this_thread::sleep_for(chrono::milliseconds(sleep_time_ms));
            this->update();
            this->draw();
        }
    });
    thread mood_thread = thread([&]()->void {
        // mood variables
        uniform_int_distribution<> interval(4, 8);
        uniform_int_distribution<> moods(1, 3);
        auto mood_length = 3000; // milli

        while (true) {
            // random mood interval
            auto sleep_time = interval(gen);

            // random mood
            auto mood = moods(gen);

            // wait until moods occurs
            this_thread::sleep_for(chrono::seconds(sleep_time));
            switch (mood) {
                case (1): // happy
                    mood_mtx.lock();
                    eye_mtx.lock();
                    mood_state = states::happy;
                    eye_state = states::happy;
                    eye_mtx.unlock();
                    mood_mtx.unlock();
                    break;
                case (2): // neutral
                    mood_mtx.lock();
                    eye_mtx.lock();
                    mood_state = states::neutral;
                    eye_state = states::neutral;
                    eye_mtx.unlock();
                    mood_mtx.unlock();
                    break;
                case (3): // angry
                    mood_mtx.lock();
                    eye_mtx.lock();
                    mood_state = states::angry;
                    eye_state = states::angry;
                    eye_mtx.unlock();
                    mood_mtx.unlock();
                    break;
                default:
                    break;
            }
        }
    });
    thread blink_thread = thread([&]()->void {
        // blink times
        uniform_int_distribution<> interval(1, 4);
        auto min = 1; // sec
        auto max = 4; // sec
        auto blink_length = 250; // milli

        while (true) {

            auto sleep_time = interval(gen);

            this_thread::sleep_for(chrono::seconds(sleep_time));

            eye_mtx.lock();
            eye_state = states::closed;
            eye_mtx.unlock();

            this_thread::sleep_for(chrono::milliseconds(blink_length));

            mood_mtx.lock();
            eye_mtx.lock();
            eye_state = mood_state;
            eye_mtx.unlock();
            mood_mtx.unlock();
        }
    });

    // functions
    void update() {
        // eyebrows
        mood_mtx.lock();
        auto browState = mood_state;
        mood_mtx.unlock();

        switch (browState) {
            case states::happy:
                mood_mtx.lock();
                eyebrows =  "´   `";
                mood_mtx.unlock();
                break;
            case states::neutral:
                mood_mtx.lock();
                eyebrows =  "-- --";
                mood_mtx.unlock();
                break;
            case states::angry:
                mood_mtx.lock();
                eyebrows = "\\   /";
                mood_mtx.unlock();
                break;
            default:
                break;
        }

        // eyes
        eye_mtx.lock();
        auto eyeState = eye_state;
        eye_mtx.unlock();

        switch (eyeState) {
            case states::happy:
                eye_mtx.lock();
                eyes = "^   ^";
                eye_mtx.unlock();
                break;
            case states::neutral:
                eye_mtx.lock();
                eyes = "0   0";
                eye_mtx.unlock();
                break;
            case states::angry:
                eye_mtx.lock();
                eyes = "*   *";
                eye_mtx.unlock();
                break;
            case states::closed:
                eye_mtx.lock();
                eyes = "-   -";
                eye_mtx.unlock();
                break;
            default:
                break;
        }
    }
    void draw() {

        // creating temp values.
        mood_mtx.lock();
        eye_mtx.lock();
        string _eyebrows = eyebrows;
        string _eyes = eyes;
        eye_mtx.unlock();
        mood_mtx.unlock();
        cout << "\n\t   -------";
        cout << "\n\t /         \\";
        cout << "\n\t|   " << _eyebrows << "   |";
        cout << "\n\t|   " << _eyes << "   |";
        cout << "\n\t|     |     |";
        cout << "\n\t|           |";
        cout << "\n\t|    ---    |";
        cout << "\n\t \\         /";
        cout << "\n\t   ------- ";
        cout << "\n\t";

        // this is so that the console isn't flooded.
        cout << "\033[10A";
    }

public:
    stickman() {
        this->mood_thread.join();
        this->blink_thread.join();
        this->print_stickman.join();
    }

};


int main(int argc, char **argv) {
    stickman p;
    return 0;
}