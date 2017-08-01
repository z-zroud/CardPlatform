#pragma once

struct IDPParser
{
    virtual int Read(const string& filePath) = 0;
    virtual void Save(const string &strPath) = 0;
};
