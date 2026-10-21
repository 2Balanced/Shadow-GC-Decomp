#ifndef GAME_TNUMEDIT_H
#define GAME_TNUMEDIT_H

/* TNumEdit and its subclasses TNumEditHex, TNumEditInt and TNumEditFloat (class names from RTTI):
 * by their names, number editors of a debug menu. Nothing else of them is known. */
class TNumEdit {
  public:
    virtual ~TNumEdit();
};

class TNumEditHex : public TNumEdit {
  public:
    virtual ~TNumEditHex();
};

class TNumEditInt : public TNumEdit {
  public:
    virtual ~TNumEditInt();
};

class TNumEditFloat : public TNumEdit {
  public:
    virtual ~TNumEditFloat();
};

/* Most game units emit the RTTI records of the three subclasses and of TNumEdit, in this order,
 * which only a dynamic_cast or typeid in inline code of the original header explains. These casts
 * reproduce those records (the original's inline code is unknown; nothing calls them). The records
 * are local: the linker keeps them only where a unit's .data is addressed through one base and so
 * is kept whole, where their base lists lead the section. */
inline TNumEditHex *AsNumEditHex(TNumEdit *edit) {
    return dynamic_cast<TNumEditHex *>(edit);
}

inline TNumEditInt *AsNumEditInt(TNumEdit *edit) {
    return dynamic_cast<TNumEditInt *>(edit);
}

inline TNumEditFloat *AsNumEditFloat(TNumEdit *edit) {
    return dynamic_cast<TNumEditFloat *>(edit);
}

#endif
