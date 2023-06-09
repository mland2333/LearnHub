#include <iostream>
#include <ctime>
#include <unistd.h>
int a = 0;
void UpdateProgress(float progress)
{
    /*int barWidth = 70;
    
    std::cout << "[";
    sleep(1);
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " <<*/

   sleep(1);
    std::cout<<"hello"; 
    std::cout<<int(progress * 100.0)<< "\r";
    std::cout.flush();
}
int main()
{
    for(int i = 0;i<100;i++)
        UpdateProgress((float)i/100);
    return 0;
}
