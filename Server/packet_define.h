#pragma once

enum class PROJECT_TYPE
{
	SERVER,
	CLIENT,

	END
};

static constexpr const PROJECT_TYPE G_PROJECT = PROJECT_TYPE::SERVER;