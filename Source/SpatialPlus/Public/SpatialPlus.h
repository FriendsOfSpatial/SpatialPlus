#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FSpatialPlusModule 
	: public IModuleInterface
{
public:
	void StartupModule() override;
	void ShutdownModule() override;
};