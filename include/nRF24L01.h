#ifndef NRF24L01_H
#define NRF24L01_H

#include "includes.h"
#include "RFProtocol.h"

/* Memory Map */
#define NRF_CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define NRF_STATUS  0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD	    0x1C
#define FEATURE	    0x1D

/* Bit Mnemonics */
#define MASK_RX_DR  6
#define MASK_TX_DS  5
#define MASK_MAX_RT 4
#define EN_CRC      3
#define CRCO        2
#define PWR_UP      1
#define PRIM_RX     0
#define ENAA_P5     5
#define ENAA_P4     4
#define ENAA_P3     3
#define ENAA_P2     2
#define ENAA_P1     1
#define ENAA_P0     0
#define ERX_P5      5
#define ERX_P4      4
#define ERX_P3      3
#define ERX_P2      2
#define ERX_P1      1
#define ERX_P0      0
#define AW          0
#define ARD         4
#define ARC         0
#define PLL_LOCK    4
#define RF_DR       3
#define RF_PWR      6
#define RX_DR       6
#define TX_DS       5
#define MAX_RT      4
#define RX_P_NO     1
#define TX_FULL     0
#define PLOS_CNT    4
#define ARC_CNT     0
#define TX_REUSE    6
#define FIFO_FULL   5
#define TX_EMPTY    4
#define RX_FULL     1
#define RX_EMPTY    0
#define DPL_P5	    5
#define DPL_P4	    4
#define DPL_P3	    3
#define DPL_P2	    2
#define DPL_P1	    1
#define DPL_P0	    0
#define EN_DPL	    2
#define EN_ACK_PAY  1
#define EN_DYN_ACK  0

/* Instruction Mnemonics */
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE      0x50
#define R_RX_PL_WID   0x60
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define REUSE_TX_PL   0xE3
#define RF24_NOP      0xFF

/* Non-P omissions */
#define LNA_HCURR   0

/* P model memory Map */
#define RPD         0x09
#define W_TX_PAYLOAD_NO_ACK  0xB0

/* P model bit Mnemonics */
#define RF_DR_LOW   5
#define RF_DR_HIGH  3
#define RF_PWR_LOW  1
#define RF_PWR_HIGH 2

typedef enum { RF24_PA_MIN = 0,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR } rf24_pa_dbm_e ;
typedef enum { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS } rf24_datarate_e;
typedef enum { RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16 } rf24_crclength_e;

#ifdef DEBUG
	#define IF_SERIAL_DEBUG(x) ({x;})
#else
	#define IF_SERIAL_DEBUG(x)
#endif

#define rf24_max(a,b) (a>b?a:b)
#define rf24_min(a,b) (a<b?a:b)

class nRF24L01 : public RFProtocol
{
public:
	nRF24L01();
	virtual ~nRF24L01() {};

	bool begin(void);
	bool isChipConnected();
	void startListening(void);
	void stopListening(void);
	bool available(void);
	void read( void* buf, uint8_t len );
	bool write( const void* buf, uint8_t len );
	void openWritingPipe(const uint8_t *address);
	void openReadingPipe(uint8_t number, const uint8_t *address);
	void printDetails(void);
	bool available(uint8_t* pipe_num);
	bool rxFifoFull();
	void powerDown(void);
	void powerUp(void) ;
	bool write( const void* buf, uint8_t len, const bool multicast );
	bool writeFast( const void* buf, uint8_t len );
	bool writeFast( const void* buf, uint8_t len, const bool multicast );
	bool writeBlocking( const void* buf, uint8_t len, uint32_t timeout );
	bool txStandBy();
	bool txStandBy(uint32_t timeout, bool startTx = 0);
	void writeAckPayload(uint8_t pipe, const void* buf, uint8_t len);
	bool isAckPayloadAvailable(void);
	void whatHappened(bool& tx_ok,bool& tx_fail,bool& rx_ready);
	void startFastWrite( const void* buf, uint8_t len, const bool multicast, bool startTx = 1 );
	void startWrite( const void* buf, uint8_t len, const bool multicast );
	void reUseTX();
	uint8_t flush_tx(void);
	bool testCarrier(void);
	bool testRPD(void) ;
	bool isValid() { return ce_pin != 0xff && csn_pin != 0xff; }
	void closeReadingPipe( uint8_t pipe ) ;
	void setAddressWidth(uint8_t a_width);
	void setRetries(uint8_t delay, uint8_t count);
	void setChannel(uint8_t channel);
	uint8_t getChannel(void);
	void setPayloadSize(uint8_t size);
	uint8_t getPayloadSize(void);
	uint8_t getDynamicPayloadSize(void);
	void enableAckPayload(void);
	void enableDynamicPayloads(void);
	void disableDynamicPayloads(void);
	void enableDynamicAck();
	bool isPVariant(void) ;
	void setAutoAck(bool enable);
	void setAutoAck( uint8_t pipe, bool enable ) ;
	void setPALevel ( uint8_t level );
	uint8_t getPALevel( void );
	bool setDataRate(rf24_datarate_e speed);
	rf24_datarate_e getDataRate( void ) ;
	void setCRCLength(rf24_crclength_e length);
	rf24_crclength_e getCRCLength(void);
	void disableCRC( void ) ;
	void maskIRQ(bool tx_ok,bool tx_fail,bool rx_ready);
	uint32_t txDelay;
	uint32_t csDelay;
	void openReadingPipe(uint8_t number, uint64_t address);
	void openWritingPipe(uint64_t address);
	uint8_t flush_rx(void);

protected:
	inline void beginTransaction();
	inline void endTransaction();


private:
	SPI spi;

	uint16_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
	uint16_t csn_pin; /**< SPI Chip select */
	uint16_t spi_speed; /**< SPI Bus Speed */
	uint8_t spi_rxbuff[32+1] ; //SPI receive buffer (payload max 32 bytes)
	uint8_t spi_txbuff[32+1] ; //SPI transmit buffer (payload max 32 bytes + 1 byte for the command)
	bool p_variant; /* False for RF24L01 and true for RF24L01P */
	uint8_t payload_size; /**< Fixed size of payloads */
	bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */
	uint8_t pipe0_reading_address[5]; /**< Last address set on pipe 0 for reading. */
	uint8_t addr_width; /**< The address width to use - 3,4 or 5 bytes. */

	void csn(bool mode);
	void ce(bool level);
	uint8_t read_register(uint8_t reg, uint8_t* buf, uint8_t len);
	uint8_t read_register(uint8_t reg);
	uint8_t write_register(uint8_t reg, const uint8_t* buf, uint8_t len);
	uint8_t write_register(uint8_t reg, uint8_t value);
	uint8_t write_payload(const void* buf, uint8_t len, const uint8_t writeType);
	uint8_t read_payload(void* buf, uint8_t len);
	uint8_t get_status(void);

#if !defined (MINIMAL)
	void print_status(uint8_t status);
	void print_observe_tx(uint8_t value);
	void print_byte_register(const char* name, uint8_t reg, uint8_t qty = 1);
	void print_address_register(const char* name, uint8_t reg, uint8_t qty = 1);
#endif
	void toggle_features(void);
	uint8_t spiTrans(uint8_t cmd);
};

#endif // NRF24L01_H
