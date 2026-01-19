#if USE_IR_RECEIVER
//#define USE_2_PULTS  //Закомментируйте эту строку, если вы используете только один пульт

// ----- настройка ИК пульта 1 -----------------------

//      Константа          код      кнопка        действие
#define IR_ON_OFF      0xFFA25D // "Power"               Вкл./Выкл. лампы
#define IR_MUTE        0xFFE21D // "Mute"                Вкл./Выкл. звук
#define IR_PREV        0xFF02FD // "Prev"                Предыдущий эффект
#define IR_NEXT        0xFFC23D // "Next"                Следущий эффект
#define IR_CYCLE       0xFF629D // "Mode"                Вкл./Выкл. избранное
#define IR_EQ          0xFFE01F // "EQ"                  Эквалайзер
#define IR_TIME        0xFF22DD // "Play/Pause"          Вывести время бег.стр.
#define IR_VOL_DOWN    0xFFA857 // "Volum-"              Уменьшить громкость
#define IR_VOL_UP      0xFF906F // "Volum+"              Увеличить громкость
#define IR_IP          0xFFB04F // "U/SD"                Показать IP адрес
#define IR_BR_UP       0xFE30CF //                    Увеличить яркость
#define IR_BR_DOWN     0xFE10EF //                    Уменьшить яркость
#define IR_SP_UP       0xFE18E7 //                    Увеличить скорость
#define IR_SP_DOWN     0xFE38C7 //                    Уменьшить скорость
#define IR_SC_UP       0xFE7A85 //                    Увеличить масштаб
#define IR_SC_DOWN     0xFE5AA5 //                    Уменьшить масштаб
#define IR_FAV_ADD     0xFE42BD //                    Добавить в избранное
#define IR_FAV_DEL     0xFE52AD //                    Удалить из избранного
#define IR_RND         0xFF9867 // "RPT"                 Случайные настр. текущего эфф.
#define IR_DEF         0xFEB04F //                    Настр. тек. эфф. по  умолчанию
#define IR_FOLD_PREV   0xFE7887 //                    Предыдущая папка
#define IR_FOLD_NEXT   0xFEF807 //                    Следующая папка
#define IR_1           0xFF30CF // "1"          1
#define IR_2           0xFF18E7 // "2"          2
#define IR_3           0xFF7A85 // "3"          3
#define IR_4           0xFF10EF // "4"          4
#define IR_5           0xFF38C7 // "5"          5 Номер эффекта
#define IR_6           0xFF5AA5 // "6"          6
#define IR_7           0xFF42BD // "7"          7
#define IR_8           0xFF4AB5 // "8"          8
#define IR_9           0xFF52AD // "9"          9
#define IR_0           0xFF6897 // "0"          0

#ifdef USE_2_PULTS

// ----- настройка ИК пульта 2 ----------------------------------

#define IR2_ON_OFF      0x40BF00FF // "Play/Pause" Вкл./Выкл. лампы
#define IR2_MUTE        0x00000001 // Вкл./Выкл. звук
#define IR2_PREV        0x40BF807F // "CH-"        Предыдущий эффект
#define IR2_NEXT        0x40BF40BF // "CH+"        Следущий эффект
#define IR2_CYCLE       0x40BF20DF // "EQ"         Вкл./Выкл. избранное
#define IR2_EQ          0x00000002 // Эквалайзер
#define IR2_TIME        0x00000003 // Сказать/показать время
#define IR2_VOL_DOWN    0x40BFA05F // "VOL-"       Уменьшить громкость
#define IR2_VOL_UP      0x40BF609F // "VOL+"       Увеличить громкость
#define IR2_IP          0x00000004 // Показать IP адрес
#define IR2_BR_UP       0x40BF50AF // "PREV"       Уменьшить яркость
#define IR2_BR_DOWN     0x40BF906F // "NEXT"       Увеличить яркость
#define IR2_SP_UP       0x00000005 // Увеличить скорость
#define IR2_SP_DOWN     0x00000006 // Уменьшить скорость
#define IR2_SC_UP       0x00000007 // Увеличить масштаб
#define IR2_SC_DOWN     0x00000008 // Уменьшить масштаб
#define IR2_FAV_ADD     0x00000009 // Добавить в избранное
#define IR2_FAV_DEL     0x0000000A // Удалить из избранного
#define IR2_RND         0x0000000B // Случайные настр. текущего эфф.
#define IR2_DEF         0x0000000C // Настр. тек. эфф. по  умолчанию
#define IR2_FOLD_PREV   0x40BF18E7 // "FL-"        Предыдущая пепка
#define IR2_FOLD_NEXT   0x40BF58A7 // "FL+"        Следующая папка
#define IR2_1           0x40BF30CF // "1"
#define IR2_2           0x40BFB04F // "2"
#define IR2_3           0x40BF708F // "3"
#define IR2_4           0x40BF08F7 // "4"
#define IR2_5           0x40BF8877 // "5"
#define IR2_6           0x40BF48B7 // "6"
#define IR2_7           0x40BF28D7 // "7"
#define IR2_8           0x40BFA857 // "8"
#define IR2_9           0x40BF6897 // "9"
#define IR2_0           0x40BF10EF // "0"
#endif //USE_2_PULTS

#endif  //USE_IR_RECEIVER
