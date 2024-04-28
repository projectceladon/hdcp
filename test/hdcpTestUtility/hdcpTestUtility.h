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
//! \file       hdcpTestUtility.h
//! \brief
//!

#ifndef __HDCP_TEST_UTILITY_H__
#define __HDCP_TEST_UTILITY_H__

#include <memory>
#include <map>
#include <string>

#include "hdcpapi.h"
#include "hdcpdef.h"

#define TYPE0_ENABLE (PORT_STATUS_HDCP_TYPE0_ENABLED | PORT_STATUS_CONNECTED)
#define TYPE1_ENABLE (PORT_STATUS_HDCP_TYPE1_ENABLED | PORT_STATUS_CONNECTED)

// hdcpTestUtility usage
#define USAGE \
    "\nUsage: hdcp_test_utility [OPTIONS]\n\
  --test [hdcp|hdcptype1]\n\
         hdcp: turn HDCP on with port maxmum supported HDCP version \n\
         hdcptype1: turn on HDCP2.2 and apply stream type restriction\n\
  --help | -h\n"

#define MAX_OPTION_COUNT 3
#define MIN_OPTION_COUNT 2

// PORT_STATUS map
static const std::map<PORT_STATUS, std::string>PortStatusMap =
{
    {PORT_STATUS_CONNECTED, "Disabled/Connected"},
    {TYPE0_ENABLE, "Type0Enabled"},
    {TYPE1_ENABLE, "Type1Enabled"},
    {PORT_STATUS_REPEATER_ATTACHED, "RepeaterAttached"},
    {PORT_STATUS_DISCONNECTED, "Invalid/Disconnected"},
};

class HdcpTestUtility
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Construct the HdcpTestUtility object
    /// \return     Nothing
    ////////////////////////////////////////////////////////////////////////////
    HdcpTestUtility();

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Destroy the HdcpTestUtility object
    /// \return     Nothing
    ////////////////////////////////////////////////////////////////////////////
    ~HdcpTestUtility(){};

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Destroy a HDCP context
    /// \return     HDCP_STATUS_SUCCESSFUL if successful, error codes on failure
    ////////////////////////////////////////////////////////////////////////////
    HDCP_STATUS HdcpDestroy(void);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Enumerate all ports connected to external display panel
    /// \return     HDCP_STATUS_SUCCESSFUL if successful, error codes on failure
    ////////////////////////////////////////////////////////////////////////////
    HDCP_STATUS HdcpEnumerateDisplay(void);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Callback function for port events
    /// \param[in]  HdcpHandle
    /// \param[in]  PortId which receive the link status event
    /// \param[in]  PortEvent
    /// \param[in]  hdcp context pointer
    /// \return     Nothing
    ////////////////////////////////////////////////////////////////////////////
    static void StatusChange(
        uint32_t   HDCPHandle,
        uint32_t   PortId,
        PORT_EVENT PortEvent,
        Context    Ctx);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Set HDCP protection protocol
    /// \param[in]  hdcpLevel
    /// \return     HDCP_STATUS_SUCCESSFUL if successful, error codes on failure
    ////////////////////////////////////////////////////////////////////////////
    HDCP_STATUS SetHdcpLevel(HDCP_LEVEL hdcpLevel);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Get HDCP status
    /// \param[out] sts receives the port status
    /// \return     HDCP_STATUS_SUCCESSFUL if successful, error codes on failure
    ////////////////////////////////////////////////////////////////////////////
    HDCP_STATUS HdcpGetStatus(PORT_STATUS sts);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Create HDCP context
    /// \return     HDCP_STATUS_SUCCESSFUL if successful, error codes on failure
    ////////////////////////////////////////////////////////////////////////////
    HDCP_STATUS HdcpCreate(void);

    ////////////////////////////////////////////////////////////////////////////
    /// \brief      Convert PORT_STATUS to string
    /// \param[in]  Status
    /// \return     string mapped to the input STATUS
    ////////////////////////////////////////////////////////////////////////////
    std::string Status2String(const PORT_STATUS Status);

private:
    PortList m_PortList;
    uint32_t m_HDCPHandle;
};

typedef std::shared_ptr<HdcpTestUtility> HdcpTestUtilityPtr;
#endif  //__HDCP_TEST_UTILITY_H__
