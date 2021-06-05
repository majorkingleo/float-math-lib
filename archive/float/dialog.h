#if !defined(MainDialog)              // Sentry, use file only if it's not already included.
#define MainDialog

#include <owl/dialog.h>
#include <owl/edit.h>



struct MainDialogTB
{
 MainDialogTB()
 {
  memset(edit1,0,sizeof(edit1));
  memset(edit2,0,sizeof(edit2));
 }
 char edit1[50];
 char edit2[1500];
};

class MainDialog : public TDialog {
  public:
    MainDialog(TWindow* parent,TResId resId=IDD_DIALOG1, TModule* module = 0,MainDialogTB* dtb=NULL);
    virtual ~MainDialog();

    TEdit*     Edit1;
    TEdit*     Edit2;
};

#endif
