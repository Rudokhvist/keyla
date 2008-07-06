//
// ћодуль ACTIVE_WINDOW_HOOK отслеживает активное окно
// и при изменении обращаетс€ к модулю CORE
//

#pragma once

namespace layoutHook {

	// Ќачать слежение за активным окном
	void create();

	// «акончить слежение за активным окном
	void destroy();
}