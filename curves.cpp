#include <pointing/pointing.h>

#include <iomanip>
#include <stdexcept>
#include <cmath>

using namespace pointing;

TransferFunction *func = 0;
TimeStamp::inttime last_time = 0;
TimeStamp::inttime first_time = 0;
bool button_pressed = false;

void
pointingCallback(void *context, TimeStamp::inttime timestamp,
                 int input_dx, int input_dy, int buttons)
{
    PointingDevice *input = reinterpret_cast<PointingDevice*>(context);
    static double mm_total;

    if (!func)
        return;

    if (first_time == 0)
        first_time = timestamp;

    double output_dx = 0,
           output_dy = 0;

    func->applyd(input_dx, input_dy, &output_dx, &output_dy, timestamp);

    double dt = (double)(timestamp - last_time)/TimeStamp::one_second;
    double cpi = input->getResolution();
    /* Convert input deltas to mm */
    double dx = 25.4/cpi * input_dx,
           dy = 25.4/cpi * input_dy;
    double mm = hypot(dx, dy); /* vector in mm */
    double speed = mm/dt; /* mm/s */

    /* Note: gain includes the "mm to pixel" conversion, speed and gain are
     * not in the same units */
    double gain = hypot(output_dx, output_dy)/mm;


    double total_time = (double)(timestamp - first_time)/TimeStamp::one_second;
    mm_total += mm;
    double speed_avg = total_time ? mm_total/total_time: 0.0;

#if 0
    printf("time: %fms in (%2d, %2d (%fmm)) out (%f, %f) gain %f speed %fmm/s"
           " total %f avg speed %f\n",
           dt * 1000, input_dx, input_dy, mm,
           output_dx, output_dy,
           gain, speed, mm_total,
           speed_avg);
#endif

    //printf("# speed(mm/s) gain\n");
    printf("%f %f\n", speed, gain);

    /* process output data with:

         cat datapoints.dat | sed "/.* 0\.000000/d" | sort -g | uniq -f 1 > datapoints-sorted.dat
     */

    last_time = timestamp;
}

int
main(int argc, char** argv) {
    try {

        if (argc < 3)
            std::cout
            << "Usage: " << argv[0] << " [inputdeviceURI [outputdeviceURI [transferfunctionURI]]]" << std::endl
            << "Using default values for some parameters" << std::endl;

        // --- Pointing device ----------------------------------------------------

        PointingDevice *input = PointingDevice::create(argc>1?argv[1]:"any:?debugLevel=1");
        for (TimeStamp reftime, now;
             !input->isActive() && now-reftime<15*TimeStamp::one_second;
             now.refresh())
            PointingDevice::idle(500);

        printf("# Pointing device: %s cpi %d@%dHz\n",
               input->getURI(true).asString().c_str(),
               input->getResolution(),
               input->getUpdateFrequency());

        // --- Display device -----------------------------------------------------

        DisplayDevice *output = DisplayDevice::create(argc>2?argv[2]:"any:?debugLevel=1");

        double hdpi, vdpi;
        output->getResolution(&hdpi, &vdpi);
        DisplayDevice::Size size = output->getSize();
        DisplayDevice::Bounds bounds = output->getBounds();

        printf("# Display device: %s %dppi %dx%d\n",
               hdpi,
               output->getURI(true).asString().c_str(),
               bounds.size.width, bounds.size.height);

        // --- Transfer function --------------------------------------------------

        func = TransferFunction::create(argc>3?argv[3]:"sigmoid:?debugLevel=2", input, output);

        printf("# Transfer function: %s\n",
               func->getURI(true).asString().c_str());

        // --- Ready to go --------------------------------------------------------

        input->setPointingCallback(pointingCallback, input);
        while (!button_pressed || 1)
            PointingDevice::idle(100); // milliseconds

        // --- Done ---------------------------------------------------------------

        delete input;
        delete output;
        delete func;

    } catch (std::runtime_error e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    } catch (std::exception e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
