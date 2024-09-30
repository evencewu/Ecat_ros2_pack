#include "ethercat_interfaces/EcatBase.hpp"
#include "ethercat_interfaces/TypeDef.hpp"
#include "ethercat_dlc/ecat_can_MW6010.hpp"

#include <array>
#include <signal.h>
#include <unistd.h>

bool app_stopped = false;

void sigint_handler(int sig);
void safe_stop();

ecat::EcatBase Ethercat(1);

std::array<ecat::MW6010dlc, 2> MotorDM = {
    ecat::MW6010dlc(CAN1, 0),
    ecat::MW6010dlc(CAN2, 0)};

int main()
{
    signal(SIGINT, sigint_handler);
 
    char phy[] = "enp5s0";
    Ethercat.EcatStart(phy);

    printf("start\n");

    
    for (int i = 0; i < 50; i++)
    {
        Ethercat.packet_tx[0].LED = 0x07;
        MotorDM[0].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::ENABLE, 0, 0, 0, 0, 0);
        Ethercat.EcatSyncMsg();

        Ethercat.packet_tx[0].LED = 0x07;
        MotorDM[0].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::SELECT_MODE, 0, 0, 0, 0, 0);
        Ethercat.EcatSyncMsg();
    }

    for (int i = 0; i < 50; i++)
    {
        Ethercat.packet_tx[0].LED = 0x07;
        MotorDM[1].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::ENABLE, 0, 0, 0, 0, 0);
        Ethercat.EcatSyncMsg();

        Ethercat.packet_tx[0].LED = 0x07;
        MotorDM[1].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::SELECT_MODE, 0, 0, 0, 0, 0);
        Ethercat.EcatSyncMsg();
    }

    
    for (int i = 0; i < 10000000; i++)
    {
        
        Ethercat.packet_tx[0].LED = 0x07;

        for (int i = 0; i < 5; i++)
        {
            MotorDM[0].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::MIT_CTRL, 0, 0, 0, 0, 1);
            Ethercat.EcatSyncMsg();
        }

        // printf("----------------\n");
        // printf("0x%hX\n", (__uint16_t)Ethercat.packet_rx[0].can[0].Data[0]);

        for (int i = 0; i < 5; i++)
        {
            MotorDM[1].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::MIT_CTRL, 0, 0, 0, 0, 1);
            Ethercat.EcatSyncMsg();
        }

        // printf("----------------\n");
        // printf("0x%hX\n", (__uint16_t)Ethercat.packet_rx[0].can[0].Data[0]);
        
        if (app_stopped)
        {
            break;
        }
        
    }

    safe_stop();
    Ethercat.EcatStop();

    printf("stop\n");

    return 0;
}

void sigint_handler(int sig)
{
    if (sig == SIGINT)
    {
        app_stopped = true;
    }
}

void safe_stop()
{
    for (int i = 0; i < 20; i++)
    {
        MotorDM[0].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::DISABLE, 0, 0, 0, 0, 0);
        Ethercat.packet_tx[0].LED = 0;
        Ethercat.EcatSyncMsg();
    }

    for (int i = 0; i < 20; i++)
    {
        MotorDM[1].MW6010_can_set(&Ethercat.packet_tx[0], ecat::MW6010dlc::DISABLE, 0, 0, 0, 0, 0);
        Ethercat.packet_tx[0].LED = 0;
        Ethercat.EcatSyncMsg();
    }

    printf("stop motor!\n");
}