/* ♥ OWT DIALOG ~ Dialogos modais faceis!
 * Dica: message mostra aviso, confirm pergunta sim/nao~
 * Overlay escuro atras, dialog no centro~
 * Confirm sempre retorna 1 agr (n tem input real) ♥ */
#ifndef OWT_DIALOG_H
#define OWT_DIALOG_H

void owt_dialog_message(const char *title, const char *message);
int owt_dialog_confirm(const char *title, const char *message);

#endif