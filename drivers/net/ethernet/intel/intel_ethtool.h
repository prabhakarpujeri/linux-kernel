/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2023 Intel Corporation. */

#ifndef _INTEL_ETHTOOL_H_
#define _INTEL_ETHTOOL_H_

#include <linux/ethtool.h>

struct net_device;

#define DEFINE_INTEL_GET_MSGLEVEL(fn_name, adapter_t) \
static u32 fn_name(struct net_device *netdev) \
{ \
	struct adapter_t *adapter = netdev_priv(netdev); \
	return adapter->msg_enable; \
}

#define DEFINE_INTEL_SET_MSGLEVEL(fn_name, adapter_t) \
static void fn_name(struct net_device *netdev, u32 data) \
{ \
	struct adapter_t *adapter = netdev_priv(netdev); \
	adapter->msg_enable = data; \
}

#define DEFINE_INTEL_GET_PAUSEPARAM(fn_name, adapter_t, hw_t, fc_rx, fc_tx, fc_full) \
static void fn_name(struct net_device *netdev, \
		    struct ethtool_pauseparam *pause) \
{ \
	struct adapter_t *adapter = netdev_priv(netdev); \
	struct hw_t *hw = &adapter->hw; \
\
	pause->autoneg = \
	    (adapter->fc_autoneg ? AUTONEG_ENABLE : AUTONEG_DISABLE); \
\
	if (hw->fc.current_mode == fc_rx) { \
		pause->rx_pause = 1; \
	} else if (hw->fc.current_mode == fc_tx) { \
		pause->tx_pause = 1; \
	} else if (hw->fc.current_mode == fc_full) { \
		pause->rx_pause = 1; \
		pause->tx_pause = 1; \
	} \
}

#define DEFINE_INTEL_GET_WOL(fn_name, adapter_t, wol_field, wufc_ex, wufc_mc, wufc_bc, wufc_mag, wufc_lnkc) \
static void fn_name(struct net_device *netdev, \
		    struct ethtool_wolinfo *wol) \
{ \
	struct adapter_t *adapter = netdev_priv(netdev); \
\
	wol->supported = WAKE_UCAST | WAKE_MCAST | \
			 WAKE_BCAST | WAKE_MAGIC; \
	if (wufc_lnkc) \
		wol->supported |= WAKE_PHY; \
\
	wol->wolopts = 0; \
\
	if (!device_can_wakeup(&adapter->pdev->dev)) \
		return; \
\
	if (adapter->wol_field & wufc_ex) \
		wol->wolopts |= WAKE_UCAST; \
	if (adapter->wol_field & wufc_mc) \
		wol->wolopts |= WAKE_MCAST; \
	if (adapter->wol_field & wufc_bc) \
		wol->wolopts |= WAKE_BCAST; \
	if (adapter->wol_field & wufc_mag) \
		wol->wolopts |= WAKE_MAGIC; \
	if (wufc_lnkc && (adapter->wol_field & wufc_lnkc)) \
		wol->wolopts |= WAKE_PHY; \
}

#define DEFINE_INTEL_SET_WOL(fn_name, adapter_t, wol_field, wufc_ex, wufc_mc, wufc_bc, wufc_mag, wufc_lnkc) \
static int fn_name(struct net_device *netdev, struct ethtool_wolinfo *wol) \
{ \
	struct adapter_t *adapter = netdev_priv(netdev); \
\
	adapter->wol_field = 0; \
\
	if (wol->wolopts & WAKE_UCAST) \
		adapter->wol_field |= wufc_ex; \
	if (wol->wolopts & WAKE_MCAST) \
		adapter->wol_field |= wufc_mc; \
	if (wol->wolopts & WAKE_BCAST) \
		adapter->wol_field |= wufc_bc; \
	if (wol->wolopts & WAKE_MAGIC) \
		adapter->wol_field |= wufc_mag; \
	if (wufc_lnkc && (wol->wolopts & WAKE_PHY)) \
		adapter->wol_field |= wufc_lnkc; \
\
	device_set_wakeup_enable(&adapter->pdev->dev, adapter->wol_field); \
\
	return 0; \
}

#define DEFINE_INTEL_GET_RINGPARAM(fn_name, adapter_t, max_rxd, max_txd) \
static void fn_name(struct net_device *netdev, \
		    struct ethtool_ringparam *ring, \
		    struct kernel_ethtool_ringparam *kernel_ring, \
		    struct netlink_ext_ack *extack) \
{ \
	struct adapter_t *adapter = netdev_priv(netdev); \
\
	ring->rx_max_pending = max_rxd; \
	ring->tx_max_pending = max_txd; \
	ring->rx_pending = adapter->rx_ring_count; \
	ring->tx_pending = adapter->tx_ring_count; \
}

#endif /* _INTEL_ETHTOOL_H_ */
