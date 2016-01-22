/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.apache.hadoop.yarn.server.api.records.impl.pb;


import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.hadoop.yarn.proto.YarnServerCommonProtos.AccStatusProto;
import org.apache.hadoop.yarn.proto.YarnServerCommonProtos.AccStatusProtoOrBuilder;
import org.apache.hadoop.yarn.proto.YarnServerCommonProtos.AcceleratorProto;
import org.apache.hadoop.yarn.server.api.records.AccStatus;
import org.apache.hadoop.yarn.server.api.records.Accelerator;

public class AccStatusPBImpl extends AccStatus {
  AccStatusProto proto = AccStatusProto.getDefaultInstance();
  AccStatusProto.Builder builder = null;
  boolean viaProto = false;
  
  private boolean alive = false;
  private boolean isUpdated = false;
  private List<Accelerator> accelerators = null;
  
  public AccStatusPBImpl() {
    builder = AccStatusProto.newBuilder();
  }

  public AccStatusPBImpl(AccStatusProto proto) {
    this.proto = proto;
    viaProto = true;
  }
  
  public synchronized AccStatusProto getProto() {
    mergeLocalToProto();
    proto = viaProto ? proto : builder.build();
    viaProto = true;
    return proto;
  }

  private synchronized void mergeLocalToBuilder() {
    if (this.accelerators != null) {
      addAcceleratorsToProto();
    }
  }

  private synchronized void mergeLocalToProto() {
    if (viaProto) 
      maybeInitBuilder();
    mergeLocalToBuilder();
    proto = builder.build();
    viaProto = true;
  }

  private synchronized void maybeInitBuilder() {
    if (viaProto || builder == null) {
      builder = AccStatusProto.newBuilder(proto);
    }
    viaProto = false;
  }
    
  private synchronized void addAcceleratorsToProto() {
    maybeInitBuilder();
    builder.clearAccelerators();
    if (accelerators == null)
      return;
    Iterable<AcceleratorProto> iterable = new Iterable<AcceleratorProto>() {
      @Override
      public Iterator<AcceleratorProto> iterator() {
        return new Iterator<AcceleratorProto>() {
  
          Iterator<Accelerator> iter = accelerators.iterator();
  
          @Override
          public boolean hasNext() {
            return iter.hasNext();
          }
  
          @Override
          public AcceleratorProto next() {
            return convertToProtoFormat(iter.next());
          }
  
          @Override
          public void remove() {
            throw new UnsupportedOperationException();
  
          }
        };
  
      }
    };
    builder.addAllAccelerators(iterable);
  }
    
  @Override
  public int hashCode() {
    return getProto().hashCode();
  }
  
  @Override
  public boolean equals(Object other) {
    if (other == null)
      return false;
    if (other.getClass().isAssignableFrom(this.getClass())) {
      return this.getProto().equals(this.getClass().cast(other).getProto());
    }
    return false;
  }

  @Override
  public synchronized boolean getAlive() {
    AccStatusProtoOrBuilder p = viaProto ? proto : builder;
    return p.getAlive();
  }
  @Override
  public synchronized void setAlive(boolean alive) {
    maybeInitBuilder();
    builder.setAlive(alive);
  }
  @Override
  public synchronized boolean getIsUpdated() {
    AccStatusProtoOrBuilder p = viaProto ? proto : builder;
    return p.getIsUpdated();
  }
  @Override
  public synchronized void setIsUpdated(boolean isUpdated) {
    maybeInitBuilder();
    builder.setIsUpdated(isUpdated);
  }

  @Override
  public List<Accelerator> getAccelerators() {
    initAccelerators();
    return this.accelerators;
  }

  @Override
  public void setAccelerators(List<Accelerator> accelerators) {
    if (accelerators == null) {
      builder.clearAccelerators();
    }
    this.accelerators = accelerators;
  }

  private void initAccelerators() {
    if (this.accelerators != null) {
      return;
    }
    AccStatusProtoOrBuilder p = viaProto ? proto : builder;
    List<AcceleratorProto> list = p.getAcceleratorsList();
    this.accelerators = new ArrayList<Accelerator>();

    for (AcceleratorProto a : list) {
      this.accelerators.add(convertFromProtoFormat(a));
    }
  }

  private Accelerator convertFromProtoFormat(AcceleratorProto proto) {
    return new AcceleratorPBImpl(proto);
  }

  private AcceleratorProto convertToProtoFormat(Accelerator a) {
    return ((AcceleratorPBImpl)a).getProto();
  }
  
  
}
