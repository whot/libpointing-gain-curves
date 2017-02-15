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
    double mm = hypot(25.4/cpi * input_dx, 25.4/cpi * input_dy); /* vector in mm */
    double speed = mm/dt; /* mm/s */

    double gain_x = input_dx ? 1.0 * output_dx/input_dx : 0;
    double gain_y = input_dy ? 1.0 * output_dy/input_dy : 0;

    /* make sure the gain is the same in both directions. since we have
     * doubles here, it should be fine, using func->applyi() gives uneven
     * gain */
    if (gain_x && gain_y) {
        if (gain_x != gain_y)
            printf("********************** gain mismatch ******************\n");
    }

    double total_time = (double)(timestamp - first_time)/TimeStamp::one_second;
    mm_total += mm;
    double speed_avg = total_time ? mm_total/total_time: 0.0;

#if 0
    printf("time: %fms in (%2d, %2d (%fmm)) out (%f, %f) gain %f speed %fmm/s"
           " total %f avg speed %f\n",
           dt * 1000, input_dx, input_dy, mm,
           output_dx, output_dy,
           gain_x, speed, mm_total,
           speed_avg);
#endif

    //printf("# speed(mm/s) gain\n");
    printf("%f %f\n", speed, gain_x);

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

        std::cout << std::endl << "Pointing device" << std::endl;
        std::cout << "  " << input->getURI(true).asString() << std::endl
        << "  " << input->getResolution() << " CPI, "
        << input->getUpdateFrequency() << " Hz" << std::endl
        << "  device is " << (input->isActive()?"":"not ") << "active" << std::endl
        << std::endl;

        // --- Display device -----------------------------------------------------

        DisplayDevice *output = DisplayDevice::create(argc>2?argv[2]:"any:?debugLevel=1");

        double hdpi, vdpi;
        output->getResolution(&hdpi, &vdpi);
        DisplayDevice::Size size = output->getSize();
        DisplayDevice::Bounds bounds = output->getBounds();
        std::cout << std::endl << "Display device" << std::endl
        << "  " << output->getURI(true).asString() << std::endl
        << "  " << bounds.size.width << " x " << bounds.size.height << " pixels, "
        << size.width << " x " << size.height << " mm" << std::endl
        << "  " << hdpi << " x " << vdpi << " PPI, "
        << output->getRefreshRate() << " Hz" << std::endl;

        // --- Transfer function --------------------------------------------------

        func = TransferFunction::create(argc>3?argv[3]:"sigmoid:?debugLevel=2", input, output);

        std::cout << std::endl << "Transfer function" << std::endl
        << "  " << func->getURI(true).asString() << std::endl
        << std::endl;

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
