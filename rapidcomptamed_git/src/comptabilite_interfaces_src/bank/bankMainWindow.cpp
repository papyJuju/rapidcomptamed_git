#include "bankMainWindow.h"
#include "deposit.h"
#include "movementschecking.h"
#include "receiptschecking.h"

#include <common/icore.h>
#include <common/position.h>

static inline Common::Position *position() {return Common::ICore::instance()->position();}

MainWindowBank::MainWindowBank(QWidget * parent)
{
    position()->centralPosition(this,width(),height());
    setupUi(this);
    setCentralWidget(new Deposits(this));
    connect(action_Recettes,SIGNAL(triggered()),this,SLOT(pointRecettes()));
    connect(action_Mouvements,SIGNAL(triggered()),this,SLOT(pointMouvements()));
    connect(actionDepotsBalance,SIGNAL(triggered()),this,SLOT(depotsEtBalance()));
        
    connect(action_Quitter,SIGNAL(triggered()),this,SLOT(close()));    
}

MainWindowBank::~MainWindowBank()
{}

void MainWindowBank::pointRecettes()
{
    ReceiptsChecking *rc = new ReceiptsChecking(this);
    setCentralWidget(rc);
}

void MainWindowBank::pointMouvements()
{
    MovementsChecking * mov = new MovementsChecking(this);
    setCentralWidget(mov);
}

void MainWindowBank::depotsEtBalance()
{
    Deposits *d = new Deposits(this);
    setCentralWidget(d);
}
