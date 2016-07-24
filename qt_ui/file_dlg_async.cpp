//**********************************************************************************
//EncryptPad Copyright 2016 Evgeny Pokhilko 
//<http://www.evpo.net/encryptpad>
//
//This file is part of EncryptPad
//
//EncryptPad is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 2 of the License, or
//(at your option) any later version.
//
//EncryptPad is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with EncryptPad.  If not, see <http://www.gnu.org/licenses/>.
//**********************************************************************************
#include "file_dlg_async.h"
#include "file_encryption.h"

using namespace EncryptPad;

FileDlgAsync::FileDlgAsync(PacketMetadata &metadata)
    :thread_(new QThread(this)), metadata_(metadata),
    result_(PacketResult::None), is_encryption_(false),
    key_service_(nullptr), kf_key_service_(nullptr)
{
    QObject::connect(thread_, SIGNAL(started()), this, SLOT(DoWork()));
    this->moveToThread(thread_);
}

void FileDlgAsync::DoWork()
{
    EncryptParams enc_params = {};

    enc_params.passphrase = !passphrase_.empty() ? &passphrase_ : nullptr;

    enc_params.key_service = key_service_;

    EncryptParams kf_encrypt_params = {};

    if(!kf_passphrase_.empty() || kf_key_service_->IsPassphraseSet())
    {
        kf_encrypt_params.passphrase = !kf_passphrase_.empty() ? &kf_passphrase_ : nullptr;
        kf_encrypt_params.key_service = kf_key_service_;
        enc_params.key_file_encrypt_params = &kf_encrypt_params;
    }

    if(is_encryption_)
    {
        result_ = EncryptPacketFile(input_file_.toStdString(), output_file_.toStdString(), 
                enc_params, metadata_);
    }
    else
    {
        result_ = DecryptPacketFile(input_file_.toStdString(), output_file_.toStdString(), 
                enc_params, metadata_);
    }

    std::fill(std::begin(passphrase_), std::end(passphrase_), '0');
    passphrase_.clear();

    std::fill(std::begin(kf_passphrase_), std::end(kf_passphrase_), '0');
    kf_passphrase_.clear();

    emit WorkDone();
    thread_->quit();
}

void FileDlgAsync::Start()
{
    thread_->start();
}

