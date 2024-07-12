/*
* Copyright (c) 2020, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
//!
//! \file       hdcpTestUtility.cpp
//! \brief
//!

#include <algorithm>
#include <cctype>
#include "hdcpTestUtility.h"

HdcpTestUtility::HdcpTestUtility()
{
    m_PortList   = {};
    m_HDCPHandle = 0;
}

void HdcpTestUtility::StatusChange(
    uint32_t   HDCPHandle,
    uint32_t   PortId,
    PORT_EVENT PortEvent,
    Context    ctx)
{
    if (nullptr == ctx)
    {
        printf("hdcp context pointer is null\n");
        return;
    }

    printf("Enter status change\n");
    switch (PortEvent)
    {
    case PORT_EVENT_PLUG_IN:
        printf("Plug out on port %d\n", PortId);
        break;
    case PORT_EVENT_PLUG_OUT:
        printf("Plug in on port %d\n", PortId);
        break;
    case PORT_EVENT_LINK_LOST:
        printf("Link lost on port %d\n", PortId);
        break;
    default:
        printf("No event on port %d\n", PortId);
        break;
    }
}

HDCP_STATUS HdcpTestUtility::HdcpDestroy(void)
{
    HDCP_STATUS ret = HDCP_STATUS_ERROR_INTERNAL;
    ret             = HDCPDestroy(m_HDCPHandle);
    return ret;
}

HDCP_STATUS HdcpTestUtility::HdcpEnumerateDisplay(void)
{
    HDCP_STATUS ret = HDCP_STATUS_ERROR_INTERNAL;

    // enumerate display
    ret = HDCPEnumerateDisplay(m_HDCPHandle, &m_PortList);
    if (HDCP_STATUS_SUCCESSFUL == ret)
    {
        if (NUM_PHYSICAL_PORTS_MAX < m_PortList.PortCount)
        {
            printf("Port count is larger than max number of ports\n");
            ret = HDCP_STATUS_ERROR_INTERNAL;
        }
        else if (0 == m_PortList.PortCount)
        {
            printf("No external display connected, please connect it!\n");
            ret = HDCP_STATUS_ERROR_NO_DISPLAY;
        }
        else
        {
            for (uint32_t i = 0; i < NUM_PHYSICAL_PORTS_MAX; ++i)
            {
                if (m_PortList.Ports[i].status & PORT_STATUS_CONNECTED)
                {
                    const char *c = (m_PortList.Ports[i].status &
                                        PORT_STATUS_REPEATER_ATTACHED)
                                        ? "repeater!"
                                        : "receiver!";
                    printf("Display through port %d is connected as a %s\n",
                        m_PortList.Ports[i].Id,
                        c);
                }
            }
        }
    }
    else
    {
        printf("Failed to enumerate display(error %d)\n", ret);
    }
    return ret;
}

HDCP_STATUS HdcpTestUtility::SetHdcpLevel(HDCP_LEVEL hdcpLevel)
{
    HDCP_STATUS ret = HDCP_STATUS_ERROR_INTERNAL;
    const char *c   = hdcpLevel ? "enable HDCP" : "disable HDCP";

    // Set HDCP level on all connected displays
    for (uint32_t i = 0; i < NUM_PHYSICAL_PORTS_MAX; ++i)
    {
        if (m_PortList.Ports[i].status & PORT_STATUS_CONNECTED)
        {
            ret = HDCPSetProtectionLevel(
                m_HDCPHandle,
                m_PortList.Ports[i].Id,
                hdcpLevel);

            if (HDCP_STATUS_SUCCESSFUL != ret)
            {
                printf("Failed to %s(error = %d) on port %d\n",
                    c,
                    ret,
                    m_PortList.Ports[i].Id);
                break;
            }
        }
    }
    if (HDCP_STATUS_SUCCESSFUL == ret)
    {
        printf("Succeed to %s\n", c);
    }
    return ret;
}

HDCP_STATUS HdcpTestUtility::HdcpGetStatus(PORT_STATUS sts)
{
    HDCP_STATUS ret     = HDCP_STATUS_ERROR_INTERNAL;
    PORT_STATUS portSts = PORT_STATUS_DISCONNECTED;

    for (uint32_t i = 0; i < NUM_PHYSICAL_PORTS_MAX; i++)
    {
        if (m_PortList.Ports[i].status & PORT_STATUS_CONNECTED)
        {
            ret = HDCPGetStatus(m_HDCPHandle, m_PortList.Ports[i].Id, &portSts);

            // check port status
            if (HDCP_STATUS_SUCCESSFUL == ret)
            {
                if (sts == portSts)
                {
                    printf("Port %d: status %s is expected\n",
                        m_PortList.Ports[i].Id,
                        Status2String(portSts).c_str());
                }
                else
                {
                    ret = HDCP_STATUS_ERROR_INTERNAL;
                    printf("Port %d: status %s is not expected(%s)\n",
                        m_PortList.Ports[i].Id,
                        Status2String(portSts).c_str(),
                        Status2String(sts).c_str());
                    break;
                }
            }
            else
            {
                printf("Port %d: failed to get status(error: %d)\n",
                    m_PortList.Ports[i].Id,
                    ret);
                ret = HDCP_STATUS_ERROR_INTERNAL;
                break;
            }
        }
    }
    return ret;
}

HDCP_STATUS HdcpTestUtility::HdcpCreate(void)
{
    HDCP_STATUS ret = HDCPCreate(&m_HDCPHandle, HdcpTestUtility::StatusChange, this);
    if (HDCP_STATUS_SUCCESSFUL != ret)
    {
        printf("HDCP context creation failed(error: %d)\n", ret);
    }
    else
    {
        printf("HDCP context creation passed\n");
    }
    return ret;
}

std::string HdcpTestUtility::Status2String(const PORT_STATUS Status)
{
    auto it = PortStatusMap.find(Status);
    if (it != PortStatusMap.end())
    {
        return it->second;
    }
    else
    {
        return "UNKNOWN";
    }
}

/**
* @Parse command.
* @param argc[in] option count
* @param argv[in] command
* @return true if valid cmd, otherwise is false.
*/
bool ParseCmdOption(int argc, char **argv)
{
    bool ret = true;

    if (MIN_OPTION_COUNT != argc && MAX_OPTION_COUNT != argc)
    {
        printf("Unrecognised option\n%s", USAGE);
        return false;
    }

    std::string sOption(argv[1]);
    std::transform(sOption.begin(), sOption.end(), sOption.begin(), ::tolower);
    if (MIN_OPTION_COUNT == argc)
    {
        if (!sOption.compare("--hdcp") || !sOption.compare("-h"))
        {
            printf("%s\n", USAGE);
        }
        else
        {
            printf("Unrecognised option\n%s", USAGE);
            ret = false;
        }
    }
    else
    {
        std::string sArg(argv[2]);
        std::transform(sArg.begin(), sArg.end(), sArg.begin(), ::tolower);
        if (!sOption.compare("--test") &&
            (!sArg.compare("hdcp") ||
                !sArg.compare("hdcptype1")))
        {
            ret = true;
        }
        else
        {
            printf("Unrecognised option\n%s", USAGE);
            ret = false;
        }
    }
    return ret;
}

int32_t main(int argc, char *argv[])
{
    HDCP_STATUS ret = HDCP_STATUS_ERROR_INTERNAL;

    // parse input parameter
    if (!ParseCmdOption(argc, argv))
    {
        return 1;
    }

    //run hdcp test with specified hdcp level
    if (MAX_OPTION_COUNT == argc)
    {
        std::string sArg(argv[2]);
        std::transform(sArg.begin(), sArg.end(), sArg.begin(), ::tolower);
        do
        {
            //1-create HDCP context
            HdcpTestUtilityPtr pHDCP = std::make_shared<HdcpTestUtility>();
            if (!pHDCP)
            {
                printf("HdcpTestUtility construction failed\n");
                break;
            }
            if (pHDCP->HdcpCreate())
            {
                printf("Please ensure hdcp service launched before test\n");
                break;
            }

            //2-enumerate display
            ret = pHDCP->HdcpEnumerateDisplay();
            if (HDCP_STATUS_SUCCESSFUL != ret)
            {
                pHDCP->HdcpDestroy();
                break;
            }

            //3-set hdcp level
            HDCP_LEVEL hdcplevel =
                sArg.compare("hdcp") ? HDCP_LEVEL2 : HDCP_LEVEL1;
            PORT_STATUS portSts =
                sArg.compare("hdcp") ? TYPE1_ENABLE : TYPE0_ENABLE;
            ret = pHDCP->SetHdcpLevel(hdcplevel);
            if (HDCP_STATUS_SUCCESSFUL == ret)
            {
                //4-check port status is expected
                ret = pHDCP->HdcpGetStatus(portSts);
            }

            //5-disable HDCP
            if (HDCP_STATUS_SUCCESSFUL == ret)
            {
                ret = pHDCP->SetHdcpLevel(HDCP_LEVEL0);
                if (HDCP_STATUS_SUCCESSFUL == ret)
                {
                    ret = pHDCP->HdcpGetStatus(PORT_STATUS_CONNECTED);
                }
            }

            //6-destroy hdcp context
            pHDCP->HdcpDestroy();
        } while (false);
        if (HDCP_STATUS_ERROR_NO_DISPLAY != ret)
        {
            if (HDCP_STATUS_SUCCESSFUL != ret)
                printf("HDCP %s test failed!\n", sArg.c_str());
            else
                printf("HDCP %s test passed!\n", sArg.c_str());
        }
    }
    return ret;
}
