#pragma once // не знаю зачем тут это но € просто оставлю это здесь

// если вы найдете ошибки с грамотностью, то не оброщ€йте внимани€ , по русскому 2 и неебЄт ^_^
// ‘айл настроек этой Simple Base
// 1 - включить
// 0 - выключить

#ifndef CONFIG_H // да да именно тут
#define CONFIG_H // и тут

#define ENABLE_SVC_HOOK 0 // включить hlr gaurd
#define ENABLE_SOUND_HOOK 0 // включить хук на PreS_DynamicSound, поставте 0 если не используете
#define ENABLE_MODULE_SECURITY 1 // скрывать dll в пам€ти cs

#define ENABLE_CHEAT_CONFIG 1 // включить конфиг дл€ чита*
#define CHEAT_CONFIG_NAME "main.cfg" // им€ файла дл€ конфига

#endif // и вот тут