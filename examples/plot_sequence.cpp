#include <iostream>
#include <string>

#define WITH_PLOTTING
#include "trignoclient.hpp"

using namespace trigno;
using namespace std::literals::chrono_literals;

void interruptHandler(int signum) {
    printf("\nUser-requested shutdown\n");
    exit(0);
}


int main(int argc, char const *argv[]) {
    // register interrupt handler
    signal(SIGINT, interruptHandler);

    // debug/verbose ouput
    printf("##################################\n%s\n##################################\n", __FILE__);

    // parse input arguments
    if (argc < 2) {
        printf("Usage: ./plot_sequence <FILE>\n");
        return 1;
    }
    auto file_path = std::string(argv[1]);

    // sort data according to timestap
    // not required, just to ensure data is time-ordered
    // data.sort();


    tools::Iterative< tools::Plotter > plotter;
    tools::Plotter plotter2;

    // configure plotter
    plotter.get().window.refresh_interval = 33ms;  // 30Hz
    plotter.get().window.autofity = false;
    // plotter.get().window().ymax = ;
    // plotter.get().window().ylim = { -0.002, 0.002 };

    // plotter2.run(duplicate.begin(20000));

    printf("---------------------\n");

    // plotter2.launch(duplicate);
    // plotter2.wait();
}
