#pragma once

#include <xdrpp/types.h>

// -------------------------------------------------------
// Primitive types and base aliases must come first
// -------------------------------------------------------
#include <xdr/Stellar-types.h>

// -------------------------------------------------------
// Contract environmental, meta, and spec definitions
// -------------------------------------------------------
#include <xdr/Stellar-contract-config-setting.h>
#include <xdr/Stellar-contract-env-meta.h>
#include <xdr/Stellar-contract-meta.h>
#include <xdr/Stellar-contract-spec.h>

// -------------------------------------------------------
// High-level entries, state, and transaction definitions
// -------------------------------------------------------
#include <xdr/Stellar-contract.h>
#include <xdr/Stellar-ledger-entries.h>
#include <xdr/Stellar-ledger.h>
#include <xdr/Stellar-transaction.h>