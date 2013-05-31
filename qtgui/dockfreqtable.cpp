#include "dockfreqtable.h"
#include "ui_dockfreqtable.h"
#include <QDir>

DockFreqTable::DockFreqTable(const QString& cfg_dir, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DockFreqTable),
    m_cfg_dir(cfg_dir)
{
    ui->setupUi(this);
    QString FreqTableDir = m_cfg_dir + "/frequency-list/";
    frequencyListTableModel = new FrequencyListTableModel(FreqTableDir);

    // Fill ComboBox
    QDir dir(FreqTableDir);
    QStringList filters;
    filters << "*.csv";
    dir.setNameFilters(filters);
    QStringList tables = dir.entryList();
    ui->comboBoxSelectFreqTable->addItems(tables);
    connect(ui->comboBoxSelectFreqTable, SIGNAL(currentIndexChanged(QString)),
            frequencyListTableModel, SLOT(load(QString)));

    // Frequency List
    ui->tableViewFrequencyList->setModel(frequencyListTableModel);
    ui->tableViewFrequencyList->setColumnWidth( FrequencyListTableModel::COL_NAME,
        ui->tableViewFrequencyList->columnWidth(FrequencyListTableModel::COL_NAME)*2 );
    ui->tableViewFrequencyList->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->tableViewFrequencyList, SIGNAL(activated(const QModelIndex &)), this, SLOT(activated(const QModelIndex &)));

    // Hide Scan Buttons
    ui->pushButtonScan->hide();
    ui->spinBoxScanSpeed->hide();

    // Update GUI
    frequencyListTableModel->load(ui->comboBoxSelectFreqTable->currentText());
}

DockFreqTable::~DockFreqTable()
{
    delete frequencyListTableModel;
    frequencyListTableModel = 0;
}


void DockFreqTable::activated(const QModelIndex & index )
{
    qint64 freq = frequencyListTableModel->table[index.row()].frequency;
    emit newFrequency(freq);
}

void DockFreqTable::setNewFrequency(qint64 rx_freq)
{
    ui->tableViewFrequencyList->clearSelection();
    for(int row=0; row<frequencyListTableModel->table.count(); ++row)
    {
        qint64 rowfreq = frequencyListTableModel->table[row].frequency;
        qint64 rowbandwidth = frequencyListTableModel->table[row].bandwidth;
        if( abs(rx_freq - rowfreq) <= ((rowbandwidth/2)+1) )
        {
            ui->tableViewFrequencyList->selectRow(row);
            ui->tableViewFrequencyList->scrollTo( ui->tableViewFrequencyList->currentIndex(), QAbstractItemView::EnsureVisible );
            break;
        }
    }
}
