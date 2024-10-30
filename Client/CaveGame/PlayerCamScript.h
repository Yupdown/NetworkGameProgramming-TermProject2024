#pragma once
#include "MonoBehavior.h"

class PlayerCamScript :
    public MonoBehavior
{
private:
public:
    PlayerCamScript();
    ~PlayerCamScript();
    
    void Start()override;
    void Update()override;
};

