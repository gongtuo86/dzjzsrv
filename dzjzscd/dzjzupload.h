#ifndef DZJZ_UPLOAD_H
#define DZJZ_UPLOAD_H

#include <atomic>
#include <QDateTime>
#include <QThread>

#include "dflogger.h"
#include "rdbscd.h"
#include "dzjzfilegenerator.h"

class DZJZ_Upload
{
    std::atomic<bool> stopFlag{false};

    static int s_nDzjzUploadTime; // 上传周期

public:
    int start()
    {
        DFLOG_INFO("[DZJZ_Inspect] Starting periodic upload service.");

        QDateTime startTime = QDateTime::currentDateTime();

        int i = 0;
        int periodInspectNum = 0;

        while (!stopFlag.load())
        {
            // 获取当前时间
            QDateTime currentTime = QDateTime::currentDateTime();
            int elapsedSeconds = startTime.secsTo(currentTime);

            if (elapsedSeconds >= s_nDzjzUploadTime)
            {
                DFLOG_INFO("[DZJZ_Inspect] Executing file export due to upload time reached.");

                startTime = QDateTime::currentDateTime();

                DZJZ_FileGenerator::DZJZ_GenRoundsFile(startTime);
                DZJZ_FileGenerator::DZJZ_GenDevicesFile(startTime);
                DZJZ_FileGenerator::DZJZ_GenLineFile(startTime);
                DZJZ_FileGenerator::DZJZ_GenActFile(startTime, s_nDzjzUploadTime);
            }

            DFLOG_INFO("[DZJZ_Inspect] Sleeping for 1 seconds.");
            sleepms(1, 0);
        }

        DFLOG_INFO("[DZJZ_Inspect] Stopping periodic upload service.");
        return 0;
    };

    void stop()
    {
        stopFlag.store(true);
    };
};

int DZJZ_Upload::s_nDzjzUploadTime = GetProgramRunPara("dzjzupload", 60);

#endif
