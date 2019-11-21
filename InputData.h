using namespace std;
#include <string>
class InputData{
    private:
        int VehicleSize;
        int UserSize;
        int MaximumRouteDuration;
        int VehicleCapacity;
    public:
        void setInputData(string inputfile);
        int getVehicleSize() const;
        int getUserSize() const;
        int getMaximumRouteDuration() const;
        int getVehicleCapacity() const;


};